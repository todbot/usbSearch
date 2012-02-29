'
' usbSearch.vbs -- Find USB devices by VID & PID
' 2012, Tod E. Kurt, http://todbot.com/
' 
' Execute on the command line with:
'   cscript.exe //nologo usbSearch.vbs /VID:<vid> /PID:<pid>
' See usage() for more options
'
' references:
' http://blogs.msdn.com/b/powershell/archive/2007/02/24/displaying-usb-devices-using-wmi.aspx
' http://technet.microsoft.com/en-us/library/ee156618.aspx
'
'
' Some notable VID/PID combos:
' - Arduino Diecimila - VID:0403 PID:6001
'
'

sub usage()
  wscript.echo "usage: usbsearch.vbs [help] [all]"
  wscript.echo "       usbsearch.vbs [quiet] [serial] /VID:<vid> [/PID:<pid>] "
  wscript.quit
end sub

strUSBsearch = ""
quiet = 0
serial = 0

for each str in wscript.arguments.unnamed
  if str = "all" or str = "/all" then strUSBsearch = "USB"
  if str = "help" or str = "/h" then usage() ' why "/h" not work?
  if str = "quiet" then quiet = 1
  if str = "serial" then serial = 1
next

set namedArgs = wscript.arguments.named
strUSBVID  = UCase(namedArgs.item("VID"))
strUSBPID  = UCase(namedArgs.item("PID"))

if strUSBsearch = "" and strUSBVID = "" and strUSBPID = "" then usage()

' set up search string
if strUSBsearch = "" then 
  if strUSBVID <> "" then 
    strUSBsearch = "USB\VID_" & strUSBVID 
    if strUSBPID <> "" then
      strUSBsearch = strUSBsearch & "&PID_" & strUSBPID 
    end if
  end if
end if

if quiet = 0 then wscript.echo "Searching for " & strUSBsearch

strComputer = "." 

set objWMIService = GetObject("winmgmts:\\" & strComputer & "\root\cimv2") 
set colDevices = objWMIService.ExecQuery _ 
   ("Select * From Win32_USBControllerDevice") 

for each objDevice in colDevices 
   strDeviceName = objDevice.Dependent 
   strQuotes = Chr(34) 
   strDeviceName = replace(strDeviceName, strQuotes, "") 
   arrDeviceNames = split(strDeviceName, "=") 
   strDeviceName = arrDeviceNames(1) 
   set colUSBDevices = objWMIService.ExecQuery _ 
       ("Select * From Win32_PnPEntity Where DeviceID = '" & _
       strDeviceName & "'") 
   for each objUSBDevice in colUSBDevices 
       if InStr( objUSBDevice.PnPDeviceId, strUSBsearch ) <> 0 then
         if serial = 1 then 
           
         end if
         if quiet = 0 then 
           wscript.echo objUSBDevice.PnPDeviceID & " - " & _
                        objUSBDevice.Description 
         else
           wscript.echo objUSBDevice.PnPDeviceID
         end if
       end if
   next
next


' Changed from Description to PnPDeviceID as this script can be altered 
' to return any property of the Win32_USBControllerDevice collection. 
