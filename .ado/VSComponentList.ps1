$installerPath = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\'
$installationPath = . $installerPath\vswhere.exe -latest -property installationPath

$p = Start-Process -PassThru $installerPath\vs_installer.exe -ArgumentList "export --installpath `"$installationPath`" --quiet --config $env:temp\vsconfig" -Wait
$p.WaitForExit()
Sleep 30
gc $env:temp\vsconfig