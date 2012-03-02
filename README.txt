usbSearch -- Search for USB devices, list USB COM ports, in Windows

2012, Tod E. Kurt, http://todbot.com/blog/


A list of Windows-only tools (for now) for inspecting USB devices & serial ports

- usbSearch.vbs    - search USB bus by VID / PID, in VBScript
- usbSearch.exe    - search USB bus by VID / PID 
- usbSearch.c      - source code for usbSearch.exe
- listComPorts.vbs - list all known COM ports, with other useful info
- listComPorts.exe - list all known COM ports, with other useful info
- listComPorts.c   - source code for listComPorts.exe
- pnpList.vbs      - list all PnP devices, USB and other
- listArduinos.pl  - Mac OS X program to list Arduino UNOs ports & serial nums

The .vbs files are executed on the command-line with the "cscript.exe" command.
For instance, to list all the COM ports, run:

cmd> cscript.exe //nologo listComPorts.vbs 
COM26 - FTDI - FTDIBUS\VID_0403+PID_6001+A6004CCFA\0000
COM24 - PJRC.COM, LLC. - USB\VID_16C0&PID_0483\12345

The exe file outputs similar results when run:

cmd> listComPorts.exe
COM26 - FTDI - FTDIBUS\VID_0403+PID_6001+A6004CCFA\0000
COM24 - PJRC.COM, LLC. - USB\VID_16C0&PID_0483\12345

The above examples show an Arduino Diecimila and Teensy plugged in.

With two Arduino UNOs, the results look like:
cmd> listComPorts.exe
COM27 - http://www.arduino.cc - USB\VID_2341&PID_0000\64936333936351400000
COM25 - http://www.arduino.cc - USB\VID_2341&PID_0000\6493234373835191F1F1


Note that the USB VID and PID are displayed (0403 and 6001, respectively, 
for the Arduino), and the serial number too, if the device has one.
In the Arduino case, the serial number is "A6004CCFA".  In the Teensy case,
the serial number is "12345".  
The serial number is useful for determining which Arduino is bound to which
COM port.

To build the exe files, install MinGW or similar and type "make".  
See the Makefile for details.

A short description of these programs is given here:
http://todbot.com/blog/2012/03/02/listcomports-windows-command-line-tool-for-usb-to-serial/





