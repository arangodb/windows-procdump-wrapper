# Corewriter

On windows it seems (please tell us if we are wrong!!) that post mortem debuggers all will write dumps to a fixed directory. On linux there is the possibility to just
specify "core-xxx" without any path. This will write the core into the current working directory. On CI systems this is super convenient as you can simply write the core
into the workspace and can later archive it. You don't have to search for specific cores of that test run (assuming there are several of these jobs).

This little thing tries to somehow emulate that behaviour on windows

## Usage

Install it as a post mortem debugger like so (elevated powershell - 64bit):

```
New-Item 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug' -type directory -force
Set-ItemProperty -Path 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug' -Name Auto -type string -Value 1
Set-ItemProperty -Path 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug' -Name Debugger -type string -Value 'C:\Procdump\corewriter.exe C:\Procdump\procdump64.exe %ld %ld %p'
```