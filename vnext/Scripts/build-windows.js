// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

const {runBuildCommand} = require('../../../packages/sdx-build-tools/lib/utils/run-build-command');

const path = require('path');
const fs = require('fs');

if (!process.env.VCINSTALLDIR) throw new Error('Cannot find Visual Studio.');
const msbuildPath = path.join(process.env.VCINSTALLDIR, 'MSBuild/15.0/Bin/msbuild.exe');
if (!fs.existsSync(msbuildPath)) throw new Error('Unable to find msbuild');

const vstestPath = path.join(process.env.VCINSTALLDIR, 'Common7/IDE/CommonExtensions/Microsoft/TestWindow/vstest.console.exe');
if (!fs.existsSync(vstestPath)) throw new Error('Unable to find vstest.console.exe');

let BuildPlatform = process.env.BuildPlatform;
if (!BuildPlatform) BuildPlatform = 'x64';

let BuildConfiguration = process.env.BuildConfiguration;
if (!BuildConfiguration) BuildConfiguration = 'Debug';

runBuildCommand('nuget', [
  'restore',
  path.resolve(__dirname, '../ReactWindows.sln'),
  '-PackagesDirectory',
  'packages',
  '-Verbosity',
  'Detailed',
  '-NonInteractive'
]);

runBuildCommand(msbuildPath, [
  path.resolve(__dirname, '../ReactWindows.sln'),
  '/nologo',
  '/nr:false',
  '/p:SolutionDir=' + path.resolve(__dirname, '..') + path.sep,
  `/p:platform=${BuildPlatform}`,
  `/p:configuration=${BuildConfiguration}`,
  '/m'
]);

const utTestDll = path.resolve(__dirname, `../target/${BuildPlatform}/${BuildConfiguration}/React.Windows.Desktop.UnitTests/React.Windows.Desktop.UnitTests.dll`)

runBuildCommand(vstestPath, [
  utTestDll,
  '/InIsolation',
  `/Platform:${BuildPlatform}`
]);

process.env.RNDesktopUTDLL = path.resolve(__dirname, `../target/${BuildPlatform}/${BuildConfiguration}/React.Windows.Desktop.IntegrationTests/React.Windows.Desktop.IntegrationTests.dll`)

require('../Scripts/run-desktop-integration-tests');
