# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

#
# Start-TestServers.ps1
#
param (
	[System.IO.DirectoryInfo] $SourcesDirectory = ($PSScriptRoot | Split-Path | Split-Path),

	[switch] $Preload,

	[int] $SleepSeconds = 10
)

Write-Host "Starting packager"

Set-Location -Path "$PSScriptRoot\..\.."

$npmCmd = (Get-Command npm.cmd -ErrorAction Stop).Source
Start-Process $npmCmd	-PassThru `
				-NoNewWindow `
				-WorkingDirectory "$PSScriptRoot\..\.." `
				-ArgumentList "run start" `
				-OutVariable packagerProc

Write-Host 'Started packager'

if ($Preload) {
	Write-Host 'Preloading bundles'

	Start-Sleep -Seconds $SleepSeconds

	# Preload the RNTesterApp integration bundle for better performance in integration tests.
	Invoke-WebRequest -UseBasicParsing -Uri "http://localhost:8081/IntegrationTests/IntegrationTestsApp.bundle?platform=windows&dev=true" | Out-Null

	Write-Host 'Done preloading bundles.'
}

# Use the environment variables input below to pass secret variables to this script.
Write-Host "##vso[task.setvariable variable=PackagerId;]$($packagerProc.Id)"
