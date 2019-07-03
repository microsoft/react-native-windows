# Copyright (c) Microsoft Corporation. All rights reserved.
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

function Stop-Packager {
	param (
	)

}

function Stop-WebSocketServer {
	param (
	)

}
