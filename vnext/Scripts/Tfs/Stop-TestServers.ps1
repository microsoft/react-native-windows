# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
#
# Stop-TestServers.ps1
#
param (
	$WebSocketServerId = $env:WebSocketServerId,
	$PackagerId = $env:PackagerId
)

Write-Host 'Stopping WebSocket server'
Stop-Process -Id $WebSocketServerId
Write-Host 'Stopped WebSocket server'

Write-Host 'Stopping packager'
Stop-Process -Id $PackagerId
Write-Host 'Stopped packager'
