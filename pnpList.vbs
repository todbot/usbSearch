'
' pnpList.vbs -- list all PNP devices, could contain COM ports
'
'

strComputer = "." 
set objWMIService = GetObject("winmgmts:\\" & strComputer & "\root\cimv2") 
set colDevices = objWMIService.ExecQuery _ 
    ("Select * From Win32_PnPEntity") 

for each objDevice in colDevices 
    wscript.echo
    wscript.echo "DeviceID:" & objDevice.DeviceID
    wscript.echo "PNPDeviceID:" & objDevice.PNPDeviceID
    wscript.echo "Caption:" & objDevice.Caption
    wscript.echo "Description:" & objDevice.Description
    wscript.echo "ConfigManagerErrorCode:" & objDevice.ConfigManagerErrorCode
next

