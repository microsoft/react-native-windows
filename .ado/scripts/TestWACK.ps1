param([string]$Platform, [string]$Configuration, [string]$OutputPath)

$exceptions = @(
);

if (!(Test-Path $OutputPath)) {
    mkdir -Path $OutputPath
}

if ($Configuration -eq 'Release') {
    $appxPath = Resolve-Path $PSScriptRoot\..\..\packages\playground\windows\AppPackages\playground\playground_1.0.0.0_${Platform}_Test\playground_1.0.0.0_$Platform.appx
    gci $appxPath

    & "C:\Program Files (x86)\Windows Kits\10\App Certification Kit\appcert.exe" test -appxpackagepath $appxPath -reportoutputpath $OutputPath\result.xml

    $xml = [xml] (gc $OutputPath\result.xml)

    $supportedApiTest = ($xml.REPORT.REQUIREMENTS.REQUIREMENT | Where-Object -Property TITLE -EQ  "Supported API test").TEST;
    $errors = $supportedApiTest.MESSAGES.MESSAGE.TEXT | Where-Object { $_ -notin $exceptions }

    if ($errors.Length -ne 0) {
        # Throw in order to break the AzureDevOps pipeline 
        throw $errors
    }
}

