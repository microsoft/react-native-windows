robocopy $env:SYSTEM_DEFAULTWORKINGDIRECTORY\ReactWindows-source $env:SYSTEM_DEFAULTWORKINGDIRECTORY\NugetRoot /E /MOVE /NP
robocopy $env:SYSTEM_DEFAULTWORKINGDIRECTORY\ReactWindows-debug-x64 $env:SYSTEM_DEFAULTWORKINGDIRECTORY\NugetRoot\target\x64\Debug /E /MOVE /NP
robocopy $env:SYSTEM_DEFAULTWORKINGDIRECTORY\ReactWindows-debug-x86 $env:SYSTEM_DEFAULTWORKINGDIRECTORY\NugetRoot\target\x86\Debug /E /MOVE /NP
robocopy $env:SYSTEM_DEFAULTWORKINGDIRECTORY\ReactWindows-ship-x64 $env:SYSTEM_DEFAULTWORKINGDIRECTORY\NugetRoot\target\x64\ship /E /MOVE /NP
robocopy $env:SYSTEM_DEFAULTWORKINGDIRECTORY\ReactWindows-ship-x86 $env:SYSTEM_DEFAULTWORKINGDIRECTORY\NugetRoot\target\x86\ship /E /MOVE /NP
robocopy $env:SYSTEM_DEFAULTWORKINGDIRECTORY\ReactWindows-debug-arm $env:SYSTEM_DEFAULTWORKINGDIRECTORY\NugetRoot\target\arm\debug /E /MOVE /NP
robocopy $env:SYSTEM_DEFAULTWORKINGDIRECTORY\ReactWindows-ship-arm $env:SYSTEM_DEFAULTWORKINGDIRECTORY\NugetRoot\target\arm\ship /E /MOVE /NP

if (Test-Path $env:SYSTEM_DEFAULTWORKINGDIRECTORY/UpdatedPackageJson/package.json) {
  $lines = Get-Content $env:SYSTEM_DEFAULTWORKINGDIRECTORY/UpdatedPackageJson/package.json | Where {$_ -match '^\s*"version":.*'} 
  $npmVersion = $lines.Trim().Split()[1].Trim('",');
} else {
  $npmVersion = "0.0.1-pr"
}
(Get-Content $env:SYSTEM_DEFAULTWORKINGDIRECTORY/NugetRoot/ReactWin32.nuspec).replace('__BuildBuildNumber__', $npmVersion) | Set-Content $env:SYSTEM_DEFAULTWORKINGDIRECTORY/NugetRoot/ReactWin32.nuspec
(Get-Content $env:SYSTEM_DEFAULTWORKINGDIRECTORY/NugetRoot/ReactUwp.nuspec).replace('__BuildBuildNumber__', $npmVersion) | Set-Content $env:SYSTEM_DEFAULTWORKINGDIRECTORY/NugetRoot/ReactUwp.nuspec
