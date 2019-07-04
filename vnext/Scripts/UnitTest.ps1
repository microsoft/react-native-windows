# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.
#
# UnitTest.ps1
#
param (
	[ValidateSet('x64', 'x86')]
	[string] $Platform = 'x64',

	[ValidateSet('Debug', 'Release')]
	[string] $Configuration = 'Debug',

	[string[]] $Tests,

	[ValidateScript({Test-Path $_})]
	[string[]] $Assemblies =
	(
		"$PSScriptRoot\..\target\$Platform\$Configuration\" +
		"React.Windows.Desktop.UnitTests\React.Windows.Desktop.UnitTests.dll"
	),

	[ValidateScript({Test-Path $_})]
	[string] $VsTest = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe"
)

# Run Unit Test assemblies.
& $VsTest ($Assemblies -join ' ') --InIsolation --Platform:$Platform ('', "--Tests:$($Tests -join ',')")[$Tests.Count -gt 0]
