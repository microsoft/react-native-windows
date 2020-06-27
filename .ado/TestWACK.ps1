param([string]$Platform, [string]$Configuration, [string]$OutputPath)

if ($Configuration -eq 'Release') {
    $appxPath = Resolve-Path $PSScriptRoot\..\packages\playground\windows\AppPackages\playground\playground_1.0.0.0_Test\playground_1.0.0.0_$Platform.appxbundle
    $appxPath

    if (!(Test-Path $OutputPath)) {
        mkdir -Path $OutputPath
    }
    & "C:\Program Files (x86)\Windows Kits\10\App Certification Kit\appcert.exe" test -appxpackagepath $appxPath -reportoutputpath $OutputPath\result.xml
}

