param (
	[Parameter(Mandatory=$true)]
	[string[]] $Features,

	[Parameter(Mandatory=$true)]
	[uri] $InstallerUri,

	[string] $VsInstaller = "${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe",

	[System.IO.FileInfo] $VsInstallPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise"
)

Invoke-WebRequest -Method Get `
	-Uri $InstallerUri `
	-OutFile $VsInstaller

Invoke-WebRequest -Method Get `
	-Uri 'https://download.microsoft.com/download/8/3/4/834E83F6-C377-4DCE-A757-69A418B6C6DF/Collect.exe' `
	-OutFile ${env:System_DefaultWorkingDirectory}\Collect.exe

Write-Host "Current VC versions:"
Get-ChildItem "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\"

$argumentList = `
	'modify',
	'--installPath', "$VsInstallPath" ,
	'--wait',
	'--quiet',
	'--norestart'

$Features | ForEach-Object {
	$argumentList += '--add', $_
}

Start-Process `
	-FilePath "$VsInstaller" `
	-ArgumentList $argumentList `
	-Wait `
	-PassThru `
	-OutVariable returnCode

# Should generate ${env:Temp}\vslogs.zip
Start-Process `
	-FilePath "${env:System_DefaultWorkingDirectory}\Collect.exe" `
	-Wait `
	-PassThru

mkdir -Force ${env:System_DefaultWorkingDirectory}\vslogs
Copy-Item ${env:TEMP}\vslogs.zip ${env:System_DefaultWorkingDirectory}\vslogs\

Write-Host "VC versions after installation:"
Get-ChildItem "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\"
