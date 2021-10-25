param([string]$PackageName, [string]$OutputDir)

$exceptions = @(
);

if (!(Test-Path $OutputDir)) {
    mkdir -Path $OutputDir
}

$packageFullName = (Get-AppxPackage $PackageName).PackageFullName
if (!$packageFullName) {
    throw "Unable to find package '$PackageName'"
}

$outputPath = Join-Path (Resolve-Path $OutputDir) result.xml
Remove-Item $outputPath -ErrorAction Ignore

Write-Host "Writing report to $outputPath"

& "${Env:ProgramFiles(x86)}\Windows Kits\10\App Certification Kit\appcert.exe" test -packagefullname $packageFullName -reportoutputpath $outputPath

$xml = [xml] (gc $outputPath)

$supportedApiTest = ($xml.REPORT.REQUIREMENTS.REQUIREMENT | Where-Object -Property TITLE -EQ  "Supported API test").TEST;
$errors = $supportedApiTest.MESSAGES.MESSAGE.TEXT | Where-Object { $_ -notin $exceptions }

if ($errors.Length -ne 0) {
    # Throw in order to break the AzureDevOps pipeline
    throw $errors
}
