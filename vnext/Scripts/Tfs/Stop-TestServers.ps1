# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
#
# Stop-TestServers.ps1
#
param (
	$PackagerId = $env:PackagerId
)

Write-Host 'Stopping packager'
Stop-Process -Id $PackagerId
Write-Host 'Stopped packager'
