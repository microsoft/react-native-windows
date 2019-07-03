# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

#
# IntegrationTests.ps1
#
param (
	[switch] $Setup,
	[switch] $NoRun,


	[ValidateSet('x64', 'x86')]
	[string] $Platform = 'x64',
	[ValidateSet('Debug', 'Release')]
	[string] $Configuration = 'Debug',
	[string[]] $Tests,


	[switch] $RequireServers = $Setup.IsPresent,


	[switch] $Cleanup,
	$Assembly =
		"$PSScriptRoot\..\target\$Platform\$Configuration\React.Windows.Desktop.IntegrationTests\React.Windows.Desktop.IntegrationTests.dll",
	$VsTest = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe",
	[UInt32] $Delay = 10,
	[switch] $List,
	[switch] $Preload,
	[string] $ReactNativeHome = "$($PSScriptRoot | Split-Path)\node_modules\react-native",


	[string[]] $Assemblies =
	(
		"$PSScriptRoot\..\target\$Platform\$Configuration\React.Windows.Desktop.IntegrationTests\React.Windows.Desktop.IntegrationTests.dll"
	),

	[ValidateScript({Test-Path $_})]
	[string] $ReactNativeLocation = "$($PSScriptRoot | Split-Path)\node_modules\react-native",

	[ValidateScript({Test-Path $_})]
	[string] $NodePath = (Get-Command node.exe).Definition,

	[ValidateScript({Test-Path $_})]
	[string] $NpmPath = (Get-Command npm.cmd).Definition
)

# Import test server functions.
. $PSScriptRoot\TestServers.ps1

function Cleanup() {
	# Expected netstat header:
	# P1 P2     P3                     P4                     P5              P6
	#    Proto  Local Address          Foreign Address        State           PID
	try {
		$process = (netstat -ano | Select-String ':8081')[0] | ConvertFrom-String
		(Get-Process -Id $process.p6).Kill()
	}
	catch {
		Write-Host 'Metro bundler process not found.'
	}

	try {
		$process = (netstat -ano | Select-String ':5555')[0] | ConvertFrom-String
		(Get-Process -Id $process.p6).Kill()
	}
	catch {
		Write-Host 'WebSocket server process not found.'
	}

	Stop-Packager

	Stop-WebSocketServer
}

#TODO: Cleanup Watchman logs, free ports 5555 and 8081.

$procs = @()

if($List.IsPresent) {
	Write-Host '-List is present. Skipping setup.'
	$Setup = $false
}

if ($Setup) {
	$procs += Start-Packager -ReactNativeLocation $ReactNativeLocation -NpmPath $NpmPath
	$procs += Start-WebSocketServer -ReactNativeLocation $ReactNativeLocation -NodePath $NodePath

	if ($Preload) {
		Start-Sleep -Seconds $Delay

		# Preload the RNTesterApp integration bundle for better performance in integration tests.
		Invoke-WebRequest -Uri "http://localhost:8081/IntegrationTests/IntegrationTestsAppWin.bundle?platform=windesktop&dev=true" | Out-Null
	}
}

if ($NoRun) {
	Exit
}

if ($List) {
	# https://github.com/Microsoft/vstest/issues/1732: Can't print full test names directly to console.
	& $VsTest $Assembly --ListFullyQualifiedTests --ListTestsTargetPath:$env:TEMP\ReactWindowsIntegrationTestsList.txt
	Get-Content $env:TEMP\ReactWindowsIntegrationTestsList.txt

	Exit
}

# Validate other options.
if ($List) {
	Write-Warning '-Run switch is present. -List will be ignored.'
}
# Ensure server ports are open.
# 5555 - String-suffixing WebSocket server
# 8081 - Packager
$ports = (5555, 8081)
foreach ($port in $ports) {
	try {
		$tcpClient = New-Object System.Net.Sockets.TcpClient
		$tcpClient.Connect("127.0.0.1", $port)
		Write-Host "Found service on TCP port $port."
	}
	catch {
		if ($RequireServers) {
			Write-Error "Test service on port $currentPort not found."
			Exit
		}

		Write-Warning 'Not all test servers found.'
	}
	finally {
		$tcpClient.Dispose()
	}
}

# Run Integration Test assemblies.
& $VsTest $Assembly --InIsolation --Platform:$Platform ('', "--Tests:$($Tests -join ',')")[$Tests.Count -gt 0]

if ($Cleanup) {
	Cleanup
}
