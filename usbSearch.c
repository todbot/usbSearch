/*
 * usbSearch -- Search for USB devices by VID and PID, shows serial numbers
 *
 * http://github.com/todbot/usbSearch/
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/
 *
 *
 * Uses DispHealper : http://disphelper.sourceforge.net/
 * Also see: 
 *  http://blogs.msdn.com/b/powershell/archive/2007/02/24/displaying-usb-devices-using-wmi.aspx
 * http://technet.microsoft.com/en-us/library/ee156618.aspx
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
	fprintf(stderr, "Usage: usbSearch [-h] [-v] [-vid <vid>] [-pid <pid>]\n");
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

    if(verbose)
        printf("Searching for '%s'...\n", vidpidstr);

    DISPATCH_OBJ(wmiSvc);
    DISPATCH_OBJ(colDevices);

    dhInitialize(TRUE);
    dhToggleExceptions(TRUE);
 
    dhGetObject(L"winmgmts:{impersonationLevel=impersonate}!\\\\.\\root\\cimv2",
                NULL, &wmiSvc);
    dhGetValue(L"%o", &colDevices, wmiSvc, 
               L".ExecQuery(%S)",  L"Select * From Win32_USBControllerDevice");



    if(0) do_silly_test();


    int match_count = 0;

    FOR_EACH(objDevice, colDevices, NULL) {
        
        char *str1 = NULL;
        char* pch;
        
        dhGetValue(L"%s", &str1,  objDevice, L".Dependent");
        
        if(verbose>1) printf("'%s'.\n", str1);
        pch = strtok( str1, "\"");
        pch = strtok( NULL, "\"");
        if(verbose>2) printf("'%s'\n", pch);
        
        char* word;
        int i;
        int match = 0;
        char vpstr[1000]; // FIXME: hack

        // pos 0 = "USB", pos 1 = "VID_01234&PID=ABCD", pos 2 = serial number
        for (word = strtok( pch, "\\" ), i=0;  word;
             word = strtok(NULL, "\\" ),i++ ) {
            if( i==0 && strstr( word, "USB" ) != NULL ) { // match
                match = 1;
            }
            if( i==1 && match==1 && strstr( word, vidpidstr ) != NULL ) {
                match = 2;
                strcpy(vpstr,word);
            }
            if( i==2 && match==2 ) {
                char vstr[5]; char pstr[5];
                sscanf( vpstr, "VID_%4s&PID_%4s", vstr, pstr);
                printf("Found VID:%s PID:%s serial:%s\n", vstr, pstr, word);
                match_count++;
            }
            if(verbose>2) printf("%d:'%s'\n", i,word );
        }
        
        dhFreeString(str1);
        
    } NEXT(objDevice);
    
    SAFE_RELEASE(colDevices);
    SAFE_RELEASE(wmiSvc);
    
    dhUninitialize(TRUE);
    
    if(verbose) 
        printf("Found %d match%s\n", match_count,(match_count==1)?"":"es");
    return 0;
}



//
int main(int argc, char **argv)
{
	parse_options(argc, argv);

    do_usbSearch();

    return 0;

}



void do_silly_test(void)
{
// could do something like this too, once you get a list of Dependents
// from Win32_USBControllerDevice
// see: http://msdn.microsoft.com/en-us/library/windows/desktop/aa387884(v=vs.85).aspx
// and: http://msdn.microsoft.com/en-us/library/windows/desktop/aa394353(v=vs.85).aspx
    DISPATCH_OBJ(aDevice);
    char *str2 = NULL;
    //    dhGetObject( L"winmgmts:\\\\BOKBOKXP\\root\\cimv2:Win32_PnPEntity.DeviceID=\"USBPRINT\\\\HPHP_COLOR_LASERJET_8500_PS\\\\6&10299E26&0&USB009\"",
    // arduino diecimilia
    dhGetObject( L"winmgmts:\\\\BOKBOKXP\\root\\cimv2:Win32_PnPEntity.DeviceID=\"USB\\\\VID_0403&PID_6001\\\\A6004CCF\"",
                 NULL, &aDevice );
    dhGetValue(L"%s", &str2,  aDevice, L".Caption");
    printf("Caption:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".CreationClassName");
    printf("CreationClassName:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".Description");
    printf("Description:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".DeviceID");
    printf("DeviceID:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".Name");
    printf("Name:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".PNPDeviceID");
    printf("PNPDeviceID:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".Status");
    printf("Status:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".SystemCreationClassName");
    printf("SystemCreationClassName:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".SystemName");
    printf("SystemName:%s\n",str2);
    //dhGetValue(L"%s", &str2,  aDevice, L".HardwareID"); // no work arrays
    //printf("HardwareID:%s\n",str2);
    dhGetValue(L"%s", &str2,  aDevice, L".Service");
    printf("Service:%s\n",str2);
    
    printf("\n\n");
}


/*

// getWmiStr:
//  Helper function to create wmi moniker incorporating computer name.
static LPWSTR getWmiStr(LPCWSTR str)
{
	static WCHAR szWmiStr[256];

	wcscpy(szWmiStr, L"winmgmts:{impersonationLevel=impersonate}!\\\\");
	wcsncat(szWmiStr, str, 128);

	return szWmiStr;
}
*/
