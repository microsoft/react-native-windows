$installerPath = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\'
$installationPath = . $installerPath\vswhere.exe -latest -property installationPath
$vsconfig = "$env:temp\vsconfig"
Write-Host "VSConfig will be at $vsconfig"
$p = Start-Process -PassThru $installerPath\vs_installer.exe -ArgumentList "export --installpath `"$installationPath`" --quiet --config $vsconfig" 
$p.WaitForExit()
$x = [Datetime]::Now.AddSeconds(60)
while (!(Test-Path .\vsconfig) -and ([datetime]::Now -lt $x))
{
    Sleep 5
    Write-Host "Waiting for vsconfig file..."
}

gc $vsconfig