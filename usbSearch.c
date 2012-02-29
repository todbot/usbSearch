/*
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>


#include <windows.h>
#include <setupapi.h>
#include <ddk/hidsdi.h>
#include <ddk/hidclass.h>

#include "disphelper.h"

//
void usage(void)
{
	fprintf(stderr, "Usage: usbSearch [-w] [-h] [-n] [-v] \n");
	fprintf(stderr, "\t-w : Wait for device to appear\n");
	fprintf(stderr, "\t-r : Use hard reboot if device not online\n");
	fprintf(stderr, "\t-n : No reboot after programming\n");
	fprintf(stderr, "\t-v : Verbose output\n");
	fprintf(stderr, "\n<MCU> = atmega32u4 | at90usb162 | at90usb646 | at90usb1286\n");
	fprintf(stderr, "\nFor more information, please visit:\n");
	exit(1);
}


// command-line options

int verbose = 0;
char* VIDstr;
char* PIDstr;


//
void parse_options(int argc, char **argv)
{
	int i;
	const char *arg;

	for (i=1; i<argc; i++) {
		arg = argv[i];
		//printf("arg: %s\n", arg);
		if (*arg == '-') { // starts with a dash
			if (strcmp(arg, "-h") == 0) {
                usage();
			} else if (strcmp(arg, "-v") == 0) {
				verbose++;
			} else if (strcmp(arg, "-vid") == 0) {
				VIDstr = argv[++i];
			} else if (strcmp(arg, "-pid") == 0) {
				PIDstr = argv[++i];
            }
		}
	}
}


// 
int do_usbSearch(void)
{
    char vidpidstr[18];
    if( VIDstr == NULL && PIDstr == NULL ) { 
        VIDstr = "-any-"; PIDstr = "-any-";
        sprintf( vidpidstr, "VID_");
    }
    else if( PIDstr == NULL ) {
        PIDstr = "-any-";
        sprintf( vidpidstr, "VID_%s", VIDstr);
    } 
    else if( VIDstr == NULL ) {
        VIDstr = "-any-";
        sprintf( vidpidstr, "PID_%s", PIDstr);
    } 
    else { 
        sprintf( vidpidstr, "VID_%s&PID_%s", VIDstr, PIDstr);
    }

    if(verbose) printf("Searching for '%s'...\n", vidpidstr);

    DISPATCH_OBJ(wmiSvc);
    DISPATCH_OBJ(colDevices);

    dhInitialize(TRUE);
    dhToggleExceptions(TRUE);
 
    dhGetObject(L"winmgmts:{impersonationLevel=impersonate}!\\\\.\\root\\cimv2",
                NULL, &wmiSvc);
    dhGetValue(L"%o", &colDevices, wmiSvc, 
               L".ExecQuery(%S)",  L"Select * From Win32_USBControllerDevice");


    int match_count = 0;

    FOR_EACH(objDevice, colDevices, NULL) {
        
        char *str1 = NULL;
        char* pch;
        
        dhGetValue(L"%s", &str1,  objDevice, L".Dependent");
 
       if(verbose>1) printf("\n'%s'.\n\n", str1);
        pch = strtok( str1, "\"");
        pch = strtok( NULL, "\"");
        if(verbose>1) printf("'%s'\n", pch);
        
        char* word;
        int i;
        int match = 0;
        char vpstr[1000];

        // pos 0 = "USB", pos 1 = "VID_01234&PID=ABCD", pos 2 = serial number
        for (word = strtok( pch, "\\" ), i=0;  word;
             word = strtok(NULL, "\\" ),i++ ) {
            if( i==0 && strstr( word, "USB" ) != NULL ) { // match
                match = 1;
            }
            if( i==1 && match==1 && strstr( word, vidpidstr ) != NULL ) { // match
                match = 2;
                strcpy(vpstr,word);
            }
            if( i==2 && match==2 ) {
                char vstr[5]; char pstr[5];
                sscanf( vpstr, "VID_%4s&PID_%4s", vstr, pstr);
                printf("Found VID:%s PID:%s serial:%s\n", vstr, pstr, word);
                match_count++;
            }
            if(verbose>1) printf("%d:'%s'\n", i,word );
        }
        
        dhFreeString(str1);
        
    } NEXT(objDevice);
    
    SAFE_RELEASE(colDevices);
    SAFE_RELEASE(wmiSvc);
    
    dhUninitialize(TRUE);
    
    if(verbose) printf("Found %d match%s\n", match_count,(match_count==1)?"":"es");
    return 0;
}



//
int main(int argc, char **argv)
{
	parse_options(argc, argv);

    do_usbSearch();

    return 0;

}
