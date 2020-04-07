param (
	[Parameter(Mandatory=$true)]
	[string[]] $Components,

	[Parameter(Mandatory=$true)]
	[uri] $InstallerUri,

	[string] $VsInstaller = "${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe",

	[System.IO.FileInfo] $VsInstallPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise",

	[System.IO.FileInfo] $VsInstallerPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer",

	[switch] $Collect

	[switch] $UseWebInstaller = $true
)

$Components | ForEach-Object {
	$componentList += '--add', $_
}

if ($UseWebInstaller) {
	Write-Host "Downloading web installer..."

	Invoke-WebRequest -Method Get `
		-Uri $InstallerUri `
		-OutFile $VsInstaller

	$VsInstallOutputDir = "${env:System_DefaultWorkingDirectory}\vs"

	New-Item -ItemType directory -Path $VsInstallOutputDir

	Write-Host "Running web installer to download requested components..."

	Start-Process `
		-FilePath "$VsInstaller" `
		-ArgumentList ( `
			'--layout', "$VsInstallOutputDir",
			'--wait',
			'--norestart',
			'--quiet' + `
			$componentList
		) `
		-Wait

	Write-Host "Running VS installer to add requested components..."

	Start-Process `
		-FilePath "$VsInstallOutputDir\vs_Enterprise.exe" `
		-ArgumentList (
			'modify',
			'--installPath', "`"$VsInstallPath`"" ,
			'--wait',
			'--norestart',
			'--quiet' + `
			$componentList
		) `
		-Wait `
		-OutVariable returnCode

	Remove-Item --path $VsInstallOutputDir
	
} else {
	Write-Host "Using local installer..."

	Start-Process `
		-FilePath "$VsInstallerPath\vs_installershell.exe" `
		-ArgumentList (
			'modify',
			'--installPath', "`"$VsInstallPath`"" ,
			'--norestart',
			'--quiet' + `
			$componentList
		) `
		-Wait `
		-OutVariable returnCode
}

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
