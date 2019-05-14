# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.
#
# UnitTest.ps1
#
param (
	[ValidateSet('x64', 'x86')]
	$Platform = 'x64',
	[ValidateSet('Debug', 'Release')]
	$Configuration = 'Debug',
	[string[]] $Tests,
	[string] $Assembly = "$PSScriptRoot\..\target\$Platform\$Configuration\React.Windows.Desktop.UnitTests\React.Windows.Desktop.UnitTests.dll",
	[string] $VsTest = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe"
)

& $VsTest $Assembly --InIsolation --Platform:$Platform ('', "--Tests:$($Tests -join ',')")[$Tests.Count -gt 0]
