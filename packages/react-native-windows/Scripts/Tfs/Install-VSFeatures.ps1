param (
	[Parameter(Mandatory=$true)]
	[string[]] $Components,

	[uri] $InstallerUri = "https://download.visualstudio.microsoft.com/download/pr/c4fef23e-cc45-4836-9544-70e213134bc8/1ee5717e9a1e05015756dff77eb27d554a79a6db91f2716d836df368381af9a1/vs_Enterprise.exe",

	[string] $VsInstaller = "${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe",

	[string] $VsInstallOutputDir = "${env:System_DefaultWorkingDirectory}\vs",

	[System.IO.FileInfo] $VsInstallPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise",

	[System.IO.FileInfo] $VsInstallerPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer",

	[switch] $Collect = $false,

	[switch] $Cleanup = $false,

	[switch] $UseWebInstaller = $false
)

$Components | ForEach-Object {
	$componentList += '--add', $_
}

$LocalVsInstaller = "$VsInstallerPath\vs_installershell.exe"

$UseWebInstaller = $UseWebInstaller -or -not (Test-Path -Path "$LocalVsInstaller")

if ($UseWebInstaller) {
	Write-Host "Downloading web installer..."

	Invoke-WebRequest `
		-UseBasicParsing `
		-Method Get `
		-Uri $InstallerUri `
		-OutFile $VsInstaller

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
		-Wait `
		-PassThru

	Write-Host "Running downloaded VS installer to add requested components..."

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
		-PassThru `
		-OutVariable returnCode

	if ($Cleanup) {
		Write-Host "Cleaning up..."

		Remove-Item -Path $VsInstaller
		Remove-Item -Path $VsInstallOutputDir -Recurse
	}
	
} else {
	Write-Host "Running local installer to add requested components..."

	Start-Process `
		-FilePath "$LocalVsInstaller" `
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
	Invoke-WebRequest `
		-UseBasicParsing `
		-Method Get `
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
