param(
   [bool]$IncludeGeneralTrace = $False
)

$scriptPath = split-path -parent $MyInvocation.MyCommand.Definition
$wprpPath = Join-Path -Path $scriptPath -ChildPath "rnw.wprp"

if (!(Get-Command "wpr.exe" -ErrorAction SilentlyContinue)) { 
   throw "
   ################################################################
   wpr.exe (Windows Performance Recorder) unavailable ! 
   Ensure that WPT (Windows Performance Toolkit) is installed and its tools are added to the path 
   ################################################################"
}

$wprArgs = "-start", $wprpPath

if ($IncludeGeneralTrace) {
   $wprArgs += "-start", "GeneralProfile"
}


Write-Host "wpr.exe $wprArgs"
& Start-Process -Verb RunAs "wpr.exe" -ArgumentList $wprArgs -PassThru -Wait

Write-Host "To stop this trace and start analysis, run .\Stop-Tracing.ps1"
Write-Host "To stop this trace only, run .\Stop-Tracing.ps1 -NoAnalysis $True"
Write-Host