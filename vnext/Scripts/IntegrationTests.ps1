# Copyright (c) Microsoft Corporation. All rights reserved.
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

	[string[]] $Tests,

	[switch] $Preload,

	[UInt32] $Delay = 10,

	[ValidateScript({Test-Path $_})]
	[string[]] $Assemblies =
	(
		"$PSScriptRoot\..\target\$Platform\$Configuration\" +
		"React.Windows.Desktop.IntegrationTests\React.Windows.Desktop.IntegrationTests.dll"
	),

	[switch] $NoRun,

	[switch] $List,

	[ValidateScript({Test-Path $_})]
	[string] $VsTest =	"${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\" +
						"Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe",

	[ValidateScript({Test-Path $_})]
	[string] $ReactNativeLocation = "$($PSScriptRoot | Split-Path | Split-Path)\node_modules\react-native",

	[ValidateScript({Test-Path $_})]
	[string] $NodePath = (Get-Command node.exe).Definition,

	[ValidateScript({Test-Path $_})]
	[string] $NpmPath = (Get-Command npm.cmd).Definition
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
	$wsServer = Find-WebSocketServer
	$notFound = $false

	if (!$packager) {
		Write-Warning 'Packager not found. Attempting to start...'
		Start-Packager -ReactNativeLocation ($PSScriptRoot | Split-Path) -NpmPath $NpmPath
		$notFound = $true
	} else {
		Write-Host 'Found Packager.'
	}

	if (!$wsServer) {
		Write-Warning 'WebSocket server not found. Attempting to start...'
		Start-WebSocketServer -ReactNativeLocation $ReactNativeLocation -NodePath $NodePath
		$notFound = $true
	} else {
		Write-Host 'Found WebSocket server.'
	}

	if ($Preload -and $notFound) {
		Start-Sleep -Seconds $Delay

		# Preload the RNTesterApp integration bundle for better performance in integration tests.
		Invoke-WebRequest -Uri "http://localhost:8081/IntegrationTests/IntegrationTestsAppWin.bundle?platform=windesktop&dev=true" | Out-Null
	}
}

if ($NoRun) {
	Exit
}

# Run Integration Test assemblies.
& $VsTest $Assemblies --InIsolation --Platform:$Platform ('', "--Tests:$($Tests -join ',')")[$Tests.Count -gt 0]
