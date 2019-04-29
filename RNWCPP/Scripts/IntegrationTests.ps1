# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

param(
	[switch] $NoSetup,
	[switch] $Run,
	[switch] $Cleanup,
	[ValidateSet('x64', 'x86')]
	[string] $Platform = 'x64',
	[ValidateSet('Debug', 'Release')]
	[string] $Configuration = 'Debug',
	[string[]] $Tests,
	$Assembly = "$PSScriptRoot\..\target\$Platform\$Configuration\React.Windows.Desktop.IntegrationTests\React.Windows.Desktop.IntegrationTests.dll",
	$VsTest = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe",
	[UInt32] $Delay = 10,
	[switch] $List,
	[switch] $Preload,
	[string] $ReactNativeHome = "$($PSScriptRoot | Split-Path)\node_modules\react-native"
)

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
}

#TODO: Cleanup Watchman logs, free ports 5555 and 8081.

$procs = @()

if($List.IsPresent) {
	Write-Host '-List is present. Skipping setup.'
	$NoSetup = $true
}

if (! $NoSetup) {
	$procs += Start-Process powershell -PassThru -WorkingDirectory "$PSScriptRoot\.." -ArgumentList "npm run start"
	$procs += Start-Process powershell -PassThru -ArgumentList '-NoProfile', "-File $ReactNativeHome\IntegrationTests\Launch-WebSocketServer.ps1 80 24"

	if ($Preload) {
		Start-Sleep -Seconds $Delay

		# Preload the RNTesterApp integration bundle for better performance in integration tests.
		Invoke-WebRequest -Uri "http://localhost:8081/IntegrationTests/IntegrationTestsAppWin.bundle?platform=windesktop&dev=true" | Out-Null
	}
}

if ($Run) {
	# Validate other options.
	if ($List.IsPresent) {
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
			Write-Error "Test service on port $currentPort not found."
			Exit
		}
		finally {
			$tcpClient.Dispose()
		}
	}
	# Run Integration Test assemblies.
	& $VsTest $Assembly --InIsolation --Platform:$Platform ('', "--Tests:$($Tests -join ',')")[$Tests.Count -gt 0]
} elseif ($List) {
	#TODO: Make -NoSetup implied.
	# https://github.com/Microsoft/vstest/issues/1732: Can't print full test names directly to console.
	& $VsTest $Assembly --ListFullyQualifiedTests --ListTestsTargetPath:$env:TEMP\ReactWindowsIntegrationTestsList.txt
	Get-Content $env:TEMP\ReactWindowsIntegrationTestsList.txt
}

if ($Cleanup) {
	Cleanup
}
