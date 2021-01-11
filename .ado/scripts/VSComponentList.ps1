$dir = $env:temp
if ($env:Agent_TempDirectory -ne $null)
{
    $dir = $env:Agent_TempDirectory
}

$installerPath = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\'
$installationPath = . $installerPath\vswhere.exe -latest -property installationPath
$vsconfig = "$dir\vsconfig"
Write-Host "VSConfig will be at $vsconfig"

Invoke-WebRequest -UseBasicParsing -Uri 'https://download.visualstudio.microsoft.com/download/pr/c4fef23e-cc45-4836-9544-70e213134bc8/1ee5717e9a1e05015756dff77eb27d554a79a6db91f2716d836df368381af9a1/vs_Enterprise.exe' -OutFile $dir\vs_enterprise.exe
$p = Start-Process -PassThru $dir\vs_enterprise.exe -RedirectStandardError $dir\err -RedirectStandardOutput $dir\out -ArgumentList "export --installpath `"$installationPath`" --quiet --config $vsconfig" 
$p.WaitForExit()
$x = [Datetime]::Now.AddSeconds(60)

do
{
    Write-Host "Waiting for vsconfig file..."
    Sleep 5
} while (!(Test-Path $vsconfig) -and ([datetime]::Now -lt $x))

Get-Content $dir\err
Get-Content $dir\out
Get-Content $vsconfig