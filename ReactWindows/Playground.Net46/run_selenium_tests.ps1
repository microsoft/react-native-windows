Add-Type -assembly “system.io.compression.filesystem”

$root = (split-path -parent $MyInvocation.MyCommand.Definition) + '\..\..'

Write-Host "Root is $root"

$reactAppDir = "$root\ReactWindows\Playground.Net46"
$winiumWorkingDir = "$reactAppDir\WiniumDriver"

Write-Host "React Native App Directory is $reactAppDir"

If (!(Test-Path $winiumWorkingDir))
{
    New-Item -ItemType directory -Path "$winiumWorkingDir"
}

Write-Host "Winium Working Directory is $winiumWorkingDir"

$targetWiniumDriverEXE = "$winiumWorkingDir\Winium.Desktop.Driver.exe"
$targetWiniumDriverZIP = "$winiumWorkingDir\Winium.Desktop.Driver.zip"

If (!(Test-Path $targetWiniumDriverEXE))
{
    $sourceWiniumDriverZIP = "https://github.com/2gis/Winium.Desktop/releases/download/v1.6.0/Winium.Desktop.Driver.zip"

    Write-Host "Winium.Desktop.Driver.zip not found - downloading latest from $targetWiniumDriverZIP"

    Invoke-WebRequest $sourceWiniumDriverZIP -OutFile $targetWiniumDriverZIP

    [io.compression.zipfile]::ExtractToDirectory($targetWiniumDriverZIP, $winiumWorkingDir)
}

Write-Host "Winium Desktop Driver EXE is $targetWiniumDriverEXE"

$winiumProcess = Start-Process -PassThru $targetWiniumDriverEXE

#Set-Location $root

npm test

Stop-Process -Id $winiumProcess.Id