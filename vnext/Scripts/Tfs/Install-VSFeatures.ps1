param (
	[Parameter(Mandatory=$true)]
	[string[]] $Features,

	[Parameter(Mandatory=$true)]
	[uri] $InstallerUri,

	[string] $VsInstaller = "${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe",

	[System.IO.FileInfo] $VsInstallPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise",

	[switch] $Collect
)

Invoke-WebRequest -Method Get `
	-Uri $InstallerUri `
	-OutFile $VsInstaller

$argumentList = `
	'modify',
	'--installPath', "`"$VsInstallPath`"" ,
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

if ($Collect) {
	Invoke-WebRequest -Method Get `
		-Uri 'https://download.microsoft.com/download/8/3/4/834E83F6-C377-4DCE-A757-69A418B6C6DF/Collect.exe' `		-Uri 'https://download.microsoft.com/download/8/3/4/834E83F6-C377-4DCE-A757-69A418B6C6DF/Collect.exe' `
		-OutFile ${env:System_DefaultWorkingDirectory}\Collect.exe

	# Should generate ${env:Temp}\vslogs.zip
	Start-Process `	Start-Process `
		-FilePath "${env:System_DefaultWorkingDirectory}\Collect.exe" `
		-Wait `
		-PassThru


	New-Item -ItemType Directory -Force ${env:System_DefaultWorkingDirectory}\vslogs

	Write-Host "VC versions after installation:"
	Get-ChildItem "$VsInstallPath\VC\Tools\MSVC\"
}
