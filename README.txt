usbSearch -- Search for USB devices 

2012, Tod E. Kurt, http://todbot.com/blog/


A list of Windows-only tools (for now) for inspecting USB devices & serial ports

- usbSearch.vbs    - search USB bus by VID / PID
- usbSearch.c      - search USB bus by VID / PID (but in C, not VBScript)
- listComPorts.vbs - list all known COM ports, with other useful info

The .vbs files are executed on the command-line with the "cscript.exe" command.
For instance, to list all the COM ports, run:

$ cscript.exe //nologo listComPorts.vbs 
COM26 - FTDI - FTDIBUS\VID_0403+PID_6001+A6004CCFA\0000
COM24 - PJRC.COM, LLC. - USB\VID_16C0&PID_0483\12345

The above example shows an Arduino Diecimila and Teensy plugged in.




