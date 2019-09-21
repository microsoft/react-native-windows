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
