$dir = $env:temp
if ($env:Agent_TempDirectory -ne $null)
{
    $dir = $env:Agent_TempDirectory
}
Write-Output "hello" |  Out-File $dir\foobar.txt
$installerPath = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\'
$installationPath = . $installerPath\vswhere.exe -latest -property installationPath
$vsconfig = "$dir\vsconfig"
Write-Host "VSConfig will be at $vsconfig"
$p = Start-Process -PassThru $installerPath\vs_installershell.exe -RedirectStandardError $dir\err -RedirectStandardOutput $dir\out -ArgumentList "export --installpath `"$installationPath`" --quiet --config $vsconfig" 
$p.WaitForExit()
$x = [Datetime]::Now.AddSeconds(60)
while (!(Test-Path $vsconfig) -and ([datetime]::Now -lt $x))
{
    Sleep 5
    Write-Host "Waiting for vsconfig file..."
}

Get-Content $dir\err
Get-Content $dir\out
Get-Content $vsconfig