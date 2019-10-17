$installerPath = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\'
$installationPath = . $installerPath\vswhere.exe -latest -property installationPath

$p = Start-Process -PassThru $installerPath\vs_installer.exe -ArgumentList "export --installpath `"$installationPath`" --quiet --config .\vsconfig" -Wait 
$p.WaitForExit()
while (!(Test-Path .\vsconfig))
{
    Sleep 5
    Write-Host "Waiting for vsconfig file..."
}

gc .\vsconfig