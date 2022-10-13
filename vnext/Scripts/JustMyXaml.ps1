# This script enables or disables VS 2022's JustMyXaml feature
[CmdletBinding()]
param([bool]$Enable)

if (!([bool](([System.Security.Principal.WindowsIdentity]::GetCurrent()).groups -match "S-1-5-32-544"))) {
    throw "Please run this script elevated";
}

$instanceId = & "$(${env:ProgramFiles(x86)})\Microsoft Visual Studio\Installer\vswhere.exe" -property instanceId
$hiveFile = "$($env:LocalAppData)\Microsoft\VisualStudio\17.0_$instanceId\privateregistry.bin"
& reg.exe load HKLM\VS2022_HIVE $hiveFile | Out-Null
New-PSDrive -Name VS2022 -PSProvider Registry -Root HKLM\VS2022_HIVE  -ErrorAction Stop | Out-Null
$currentValue = (Get-ItemProperty VS2022:\Software\Microsoft\VisualStudio\17.0_$instanceId\Debugger -Name EnableXamlVisualDiagnosticsJustMyXaml).EnableXamlVisualDiagnosticsJustMyXaml
if ($currentValue -eq 0) { $currentValue = $false; } else { $currentValue = $true; }
Write-Host "Current value: $currentValue"
if ($Enable) { $newValue = 1; } else { $newValue = 0; }
Set-ItemProperty VS2022:\Software\Microsoft\VisualStudio\17.0_$instanceId\Debugger -Name EnableXamlVisualDiagnosticsJustMyXaml -Value $newValue -Type DWord
Write-Host "New value: $Enable"
Remove-PSDrive -Name VS2022
& reg.exe unload HKLM\VS2022_HIVE | Out-Null
