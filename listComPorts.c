/*
 * listComPorts.c -- list COM ports
 *
 * http://github.com/todbot/usbSearch/
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/
 *
 *
 * Uses DispHealper : http://disphelper.sourceforge.net/
 *
 * Notable VIDs & PIDs combos:
 * VID 0403 - FTDI
 * 
 * VID 0403 / PID 6001 - Arduino Diecimila
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
	fprintf(stderr, "Usage: listComPorts [-h] [-v] [-vid <vid>] [-pid <pid>]\n");
	fprintf(stderr, "\t-vid <vid> : Search by Vendor ID\n");
	fprintf(stderr, "\t-pid <pid> : Search by Product ID\n");
	fprintf(stderr, "\t-v : Verbose output (more for more output)\n");
	fprintf(stderr, "\t-h : This help message\n");
	fprintf(stderr, "\nFor more info, https://github.com/todbot/usbSearch\n");
	exit(1);
}


// command-line options
int verbose = 0;
char* VIDstr;
char* PIDstr;

void do_silly_test(void);

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
int listComPorts(void)
{
    if(verbose)
        printf("Searching for COM ports...\n");

    DISPATCH_OBJ(wmiSvc);
    DISPATCH_OBJ(colDevices);

    dhInitialize(TRUE);
    dhToggleExceptions(TRUE);
 
    dhGetObject(L"winmgmts:{impersonationLevel=impersonate}!\\\\.\\root\\cimv2",
    //dhGetObject(L"winmgmts:\\\\.\\root\\cimv2",
                NULL, &wmiSvc);
    dhGetValue(L"%o", &colDevices, wmiSvc, 
               L".ExecQuery(%S)",  
               L"Select * from Win32_PnPEntity");


    int port_count = 0;

    FOR_EACH(objDevice, colDevices, NULL) {
        
        char* name = NULL;
        char* pnpid = NULL;
        char* manu = NULL;
        char* match;

        dhGetValue(L"%s", &name,  objDevice, L".Name");
        dhGetValue(L"%s", &pnpid, objDevice, L".PnPDeviceID");
                                                
        if(verbose>1) printf("'%s'.\n", name);
        if( name != NULL && ((match = strstr( name, "(COM" )) != NULL) ) { // look for "(COM23)"
            // 'Manufacturuer' can be null, so only get it if we need it
            dhGetValue(L"%s", &manu, objDevice,  L".Manufacturer");
            port_count++;
            char* comname = strtok( match, "()");
            printf("%s - %s - %s\n",comname, manu, pnpid);
            dhFreeString(manu);
        }
        
        dhFreeString(name);
        dhFreeString(pnpid);
        
    } NEXT(objDevice);
    
    SAFE_RELEASE(colDevices);
    SAFE_RELEASE(wmiSvc);
    
    dhUninitialize(TRUE);
    
    if(verbose) 
        printf("Found %d port%s\n",port_count,(port_count==1)?"":"s");
    return 0;
}



//
int main(int argc, char **argv)
{
	parse_options(argc, argv);

    listComPorts();

    return 0;

}


