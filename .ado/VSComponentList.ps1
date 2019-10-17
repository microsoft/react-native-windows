$p = Start-Process -PassThru 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe' -ArgumentList "export --installpath `"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise`" --quiet --config $env:temp\vsconfig"
$p.WaitForExit()
gc $env:temp\vsconfig