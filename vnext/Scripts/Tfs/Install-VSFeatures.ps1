param (
	[Parameter(Mandatory=$true)]
	[string[]] $Components,

	[Parameter(Mandatory=$true)]
	[uri] $InstallerUri,

	[string] $VsInstaller = "${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe",

	[System.IO.FileInfo] $VsInstallPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise",

	[switch] $Collect
)

Write-Host "Downloading web installer..."

Invoke-WebRequest -Method Get `
	-Uri $InstallerUri `
	-OutFile $VsInstaller

$Components | ForEach-Object {
	$componentList += '--add', $_
}

$VsInstallOutputDir = "${env:System_DefaultWorkingDirectory}\vs"

New-Item -ItemType directory -Path $VsInstallOutputDir

Write-Host "Running web installer to download components..."

Start-Process `
	-FilePath "$VsInstaller" `
	-ArgumentList ( `
		'--layout', "$VsInstallOutputDir",
		'--wait',
		'--norestart',
		'--verbose',
		'--quiet' + `
		$componentList
	) `
	-Wait `
	-PassThru

Write-Host "Running actual VS installer..."

Start-Process `
	-FilePath "$VsInstallOutputDir\vs_Enterprise.exe" `
	-ArgumentList (
		'modify',
		'--installPath', "`"$VsInstallPath`"" ,
		'--wait',
		'--norestart'
		'--verbose',
		'--quiet' + `
		$componentList
	) `
	-Wait `
	-PassThru `
	-OutVariable returnCode

if ($Collect) {
	Invoke-WebRequest -Method Get `
		-Uri 'https://download.microsoft.com/download/8/3/4/834E83F6-C377-4DCE-A757-69A418B6C6DF/Collect.exe' `
		-OutFile ${env:System_DefaultWorkingDirectory}\Collect.exe

	# Should generate ${env:Temp}\vslogs.zip
	Start-Process `
		-FilePath "${env:System_DefaultWorkingDirectory}\Collect.exe" `
		-Wait `
		-PassThru

	New-Item -ItemType Directory -Force ${env:System_DefaultWorkingDirectory}\vslogs
	Expand-Archive -Path ${env:TEMP}\vslogs.zip -DestinationPath ${env:System_DefaultWorkingDirectory}\vslogs\

	Write-Host "VC versions after installation:"
	Get-ChildItem -Name "$VsInstallPath\VC\Tools\MSVC\"
}
