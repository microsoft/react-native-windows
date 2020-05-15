# This script enables or disables VS 2019's JustMyXaml feature
[CmdletBinding()]
param([bool]$Enable)
$instanceId = & "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -property instanceId
$hiveFile = "$($env:LocalAppData)\Microsoft\VisualStudio\16.0_$instanceId\privateregistry.bin"
& reg.exe load HKLM\VS2019_HIVE $hiveFile | Out-Null
New-PSDrive -Name VS2019 -PSProvider Registry -Root HKLM\VS2019_HIVE  -ErrorAction Stop | Out-Null
$currentValue = (Get-ItemProperty VS2019:\Software\Microsoft\VisualStudio\16.0_9f8d4bd3\Debugger -Name EnableXamlVisualDiagnosticsJustMyXaml).EnableXamlVisualDiagnosticsJustMyXaml
if ($currentValue -eq 0) { $currentValue = $false; } else { $currentValue = $true; }
Write-Host "Current value: $currentValue"
if ($Enable) { $newValue = 1; } else { $newValue = 0; }
Set-ItemProperty VS2019:\Software\Microsoft\VisualStudio\16.0_9f8d4bd3\Debugger -Name EnableXamlVisualDiagnosticsJustMyXaml -Value $newValue -Type DWord
Write-Host "New value: $Enable"
Remove-PSDrive -Name VS2019
& reg.exe unload HKLM\VS2019_HIVE | Out-Null
