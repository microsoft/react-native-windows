# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

#
# IntegrationTests.ps1
#
param (
	[Parameter(HelpMessage="Don't start nor use test servers.")]
	[switch] $NoServers,

	[ValidateSet('x64', 'x86')]
	[string] $Platform = 'x64',

	[ValidateSet('Debug', 'Release')]
	[string] $Configuration = 'Debug',

	[string[]] $Include,

	[string[]] $Exclude,

	[switch] $Preload,

	[UInt32] $Delay = 10,

	[System.IO.FileInfo[]] $Assemblies =
	(
		("$(Split-Path $PSScriptRoot)\target\$Platform\$Configuration\" +
		"React.Windows.Desktop.IntegrationTests\React.Windows.Desktop.IntegrationTests.dll")
	),

	[switch] $NoRun,

	[switch] $List,

	[System.IO.FileInfo] $VsTest =
		"$(& "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath)\" +
		"Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe",

	[System.IO.DirectoryInfo] $ReactNativeLocation = "$($PSScriptRoot | Split-Path)",

	[System.IO.FileInfo] $DotNetPath = (Get-Command dotnet.exe).Definition,

	[System.IO.FileInfo] $NpmPath = (Get-Command npm.cmd).Definition
)

# Import test server functions.
. $PSScriptRoot\TestServers.ps1

if ($List) {
	# https://github.com/Microsoft/vstest/issues/1732: Can't print full test names directly to console.
	& $VsTest ($Assemblies -join ' ') --ListFullyQualifiedTests `
		--ListTestsTargetPath:$env:TEMP\ReactWindowsIntegrationTestsList.txt

	Get-Content $env:TEMP\ReactWindowsIntegrationTestsList.txt
	Exit
}

# Ensure test services are online.
if (! $NoServers) {
	$packager = Find-Packager
	$webSite = Find-TestWebsiteServer
	$notFound = $false

	if (!$packager) {
		Write-Warning 'Packager not found. Attempting to start...'
		Start-Packager -ReactNativeLocation $ReactNativeLocation -NpmPath $NpmPath
		$notFound = $true
	} else {
		Write-Host 'Found Packager.'
	}

	if (!$webSite) {
		Start-Process `
			-FilePath $DotNetPath `
			-PassThru `
			-ArgumentList `
				run,
				--project,
				$ReactNativeLocation\TestWebSite\Microsoft.ReactNative.Test.Website.csproj

		$notFound = $true
	} else {
		Write-Host 'Found Test Website Server'
	}

	if ($Preload -and $notFound) {
		Start-Sleep -Seconds $Delay

		# Ensure test website responds to default HTTP GET request.
		Invoke-WebRequest -Uri 'http://localhost:5555'

		# Preload the RNTesterApp integration bundle for better performance in integration tests.
		Invoke-WebRequest -UseBasicParsing -Uri "http://localhost:8081/IntegrationTests/IntegrationTestsApp.bundle?platform=windows&dev=true" | Out-Null
	}
}

if ($NoRun) {
	Exit
}

$filter = ''
if ($Include.Count) {
	$filter += "(FullyQualifiedName~" + ($Include -join ')&(FullyQualifiedName~') + ")"
}

if ($Exclude.Count) {
	$filter += ('', '&')[$filter.Length -gt 0] + "(FullyQualifiedName!~" + ($Exclude -join ')&(FullyQualifiedName!~') + ")"
}

# Run Integration Test assemblies.
if ($filter.Length -gt 0) {
	& $VsTest $Assemblies --InIsolation --Platform:$Platform "--TestCaseFilter:$filter"
} else {
	& $VsTest $Assemblies --InIsolation --Platform:$Platform
}
