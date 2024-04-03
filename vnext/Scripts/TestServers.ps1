# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

#
# TestServers.ps1
#
function Start-Npm {
	param (
		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $WorkingDirectory,

		[switch] $NoNewWindow,

		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $NpmPath
	)

	return Start-Process `
		-FilePath $NpmPath `
		-PassThru `
		-NoNewWindow:$NoNewWindow `
		-WorkingDirectory $WorkingDirectory `
		-ArgumentList 'run', 'start'
}

function Start-Node {
	param (
		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $ScriptPath,

		[switch] $NoNewWindow,

		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $NodePath
	)

	return Start-Process `
		-FilePath $NodePath `
		-PassThru `
		-NoNewWindow:$NoNewWindow `
		-ArgumentList $ScriptPath
}

function Start-Packager {
	param (
		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $ReactNativeLocation,

		[switch] $NoNewWindow,

		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $NpmPath
	)

	return Start-Npm -WorkingDirectory $ReactNativeLocation -NoNewWindow:$NoNewWindow -NpmPath $NpmPath
}

function Start-WebSocketServer {
	param (
		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $ReactNativeLocation,

		[switch] $NoNewWindow,

		[Parameter(Mandatory=$true)]
		[ValidateScript({Test-Path $_})]
		[string] $NodePath
	)

	return Start-Node	-ScriptPath $ReactNativeLocation\IntegrationTests\websocket_integration_test_server.js `
						-NoNewWindow:$NoNewWindow `
						-NodePath $NodePath
}

function Find-Packager {
	try {
		return Get-Process -Id (Get-NetTCPConnection -ErrorAction Ignore -LocalPort 8081).OwningProcess
	}
	catch {
		return $null
	}
}

function Find-WebSocketServer {
	try {
		return Get-Process -Id (Get-NetTCPConnection -ErrorAction Ignore -LocalPort 5555).OwningProcess
	}
	catch {
		return $null
	}
}

function Stop-WebSocketServer {
	$proc = Find-WebSocketServer

	if ($proc) {
		Stop-Process $proc
	}
}

function Stop-Packager {
	$proc = Find-Packager

	if ($proc) {
		Stop-Process $proc
	}
}
