# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.
#
# Bootstrap.ps1
# Run from directory 'src\react-native-win' under the repository root.
#
param (
  $Git     = 'git',
  $MSBuild = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\MSBuild.exe",
  $rush    = 'node common/scripts/install-run-rush.js'
  $Nuget   = 'NuGet'
)

$rootDir = $PSScriptRoot | Split-Path | Split-Path
Set-Location $rootDir

# Restore Git submoudles.
& $Git submodule update --init

# Install NPM modules.
& $rush install

Set-Location $rootDir\react-native-win

# Restore packages.
& $NuGet restore
& $MSBuild /t:Restore
