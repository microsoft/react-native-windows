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
					-WorkingDirectory = ({Get-Location}, {$WorkingDirectory})[$WorkingDirectory] `
					-Verb 'run start' `
					-OutVariable npmProcessId

	return $npmProcessId
}

function Start-Node {
	param (
		[string] $ScriptPath,
		[string] $NodePath = (Get-Command node.exe).Definition,
		[switch] $NoNewWindow
	)

	Start-Process	-FilePath $NodePath `
					-PassThru `
					-NoNewWindow $NoNewWindow `
					-ArgumentList $ScriptPath `
					-OutVariable nodeProcId

	return $nodeProcId
}
