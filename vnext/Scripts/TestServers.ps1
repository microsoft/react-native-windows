# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

#
# TestServers.ps1
#
function Start-Npm {
	param (
		[string] $ReactNativeLocation = "$($PSScriptRoot | Split-Path | Split-Path)\node_modules\react-native",
		[string] $NpmPath = (Get-Command npm.cmd).Definition,
		[switch] $NoNewWindow,
		[string] $WorkingDirectory
	)

	Start-Process	-FilePath $NpmPath `
					-PassThru `
					-NoNewWindow $NoNewWindow `
					-WorkingDirectory = ({Get-Location}, {$WorkingDirectory})[$WorkingDirectory]
}

function Start-Node {
	param (
		[string] $NodePath = (Get-Command node.exe).Definition
	)


}

Push-Location
Set-Location -Path $ReactNativeLocation

Start-Process	-FilePath (Get-Command node.exe).Definition `
				-PassThru `
				-NoNewWindow `
				-WorkingDirectory $ReactNativeLocation `
				-ArgumentList "run start" `
				-OutVariable packagerProc

Write-Host 'Started packager'

Write-Host 'Starting WebSocket server'

Start-Process	-FilePath (Get-Command node.exe).Definition `
				-PassThru `
				-ArgumentList "${SourcesDirectory}\IntegrationTests\websocket_integration_test_server.js" `
				-OutVariable wsProc

Write-Host 'Started WebSocket server'

if ($Preload) {
	Write-Host 'Preloading bundles'

	Start-Sleep -Seconds 12

	# Preload the RNTesterApp integration bundle for better performance in integration tests.
	Invoke-WebRequest -Uri "http://localhost:8081/IntegrationTests/IntegrationTestsAppWin.bundle?platform=windesktop&dev=true" | Out-Null

	Write-Host 'Done preloading bundles.'
}

Pop-Location

return ($packagerProc, $wsProc)
