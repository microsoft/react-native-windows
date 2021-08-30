param(
    [bool]$NoAnalysis = $False
)

$mypath = split-path -parent $MyInvocation.MyCommand.Definition
$timestamp = [DateTime]::Now.ToString("yyyyMMdd_HHmmss") 
$fileName = "rnw_" + $timestamp + ".etl"
$etlPath = Join-Path -Path $mypath -ChildPath $fileName

if (!(Get-Command "wpr.exe" -ErrorAction SilentlyContinue)) { 
    throw "
   ################################################################
   wpr.exe (Windows Performance Recorder) unavailable ! 
   Ensure that WPT (Windows Performance Toolkit) is installed and its tools are added to the path 
   ################################################################"
}
 
$wprArgs = "-stop", $etlPath

Write-Host "wpr.exe $wprArgs"
& Start-Process -Verb RunAs "wpr.exe" -ArgumentList $wprArgs -PassThru -Wait

if (!(Get-Command "wpa.exe" -ErrorAction SilentlyContinue)) { 
    Write-Host "wpa.exe (Windows Performance Analyzer) unavailable !"
    Write-Host "Ensure that WPT (Windows Performance Toolkit) is installed and its tools are added to the path "
}

if ($NoAnalysis) {
    Write-Host "To analys this trace, install WPT and run: 'wpa $etlPath'"
}
else {

    if (!(Get-Command "wpa.exe" -ErrorAction SilentlyContinue)) { 
        throw "
        ################################################################
        wpa.exe (Windows Performance Analyzer) unavailable ! 
        Ensure that WPT (Windows Performance Toolkit) is installed and its tools are added to the path 
        ################################################################"
    }

    $wpaArgs = $etlPath
    Write-Host "wpa.exe $wpaArgs"
    & Start-Process -Verb RunAs "wpa.exe" -ArgumentList $wpaArgs
}


