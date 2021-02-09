/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';

import {
  projectConfigWindows,
  WindowsProjectConfig,
} from '../config/projectConfig';

import {copyAndReplace} from '../generator-common';
import {ensureWinUIDialect} from '../runWindows/utils/autolink';

const templateRoot = path.resolve('../../../vnext/template');

const testProjectGuid = '{416476D5-974A-4EE2-8145-4E331297247E}';

async function tryMkdir(dir: string): Promise<void> {
  try {
    await fs.promises.mkdir(dir, {recursive: true});
  } catch (err) {}
}

type TargetProject = [string, ((folder: string) => Promise<void>)?];

function project(
  name: string,
  setup?: (folder: string) => Promise<void>,
): TargetProject {
  return [name, setup];
}

const projects: TargetProject[] = [
  // Nothing but a react-native.config.js, with nulls
  project('BlankApp'),
  // Nothing but a windows folder
  project('MissingProjectFilesApp', async (folder: string) => {
    const windowsDir = path.join(folder, 'windows');
    await tryMkdir(windowsDir);
  }),
  // New C++ project based on the template
  project('SimpleCppApp', async (folder: string) => {
    const windowsDir = path.join(folder, 'windows');
    await tryMkdir(windowsDir);

    const replacements = {
      name: 'SimpleCppApp',
      namespace: 'SimpleCppApp',
      useMustache: true,
      projectGuidUpper: testProjectGuid,
      projectGuidLower: testProjectGuid.toLowerCase(),
    };

    await copyAndReplace(
      path.join(templateRoot, 'cpp-app/proj/MyApp.sln'),
      path.join(windowsDir, 'SimpleCppApp.sln'),
      replacements,
      null,
    );

    const projDir = path.join(windowsDir, 'SimpleCppApp');
    await tryMkdir(projDir);

    await copyAndReplace(
      path.join(templateRoot, 'cpp-app/proj/MyApp.vcxproj'),
      path.join(projDir, 'SimpleCppApp.vcxproj'),
      replacements,
      null,
    );
  }),
  // New C# project based on the template
  project('SimpleCSharpApp', async (folder: string) => {
    const windowsDir = path.join(folder, 'windows');
    await tryMkdir(windowsDir);

    const replacements = {
      name: 'SimpleCSharpApp',
      namespace: 'SimpleCSharpApp',
      useMustache: true,
      projectGuidUpper: testProjectGuid,
      projectGuidLower: testProjectGuid.toLowerCase(),
    };

    await copyAndReplace(
      path.join(templateRoot, 'cs-app/proj/MyApp.sln'),
      path.join(windowsDir, 'SimpleCSharpApp.sln'),
      replacements,
      null,
    );

    const projDir = path.join(windowsDir, 'SimpleCSharpApp');
    await tryMkdir(projDir);

    await copyAndReplace(
      path.join(templateRoot, 'cs-app/proj/MyApp.csproj'),
      path.join(projDir, 'SimpleCSharpApp.csproj'),
      replacements,
      null,
    );
  }),
  project('WithWinUI3', async (folder: string) => {
    const windowsDir = path.join(folder, 'windows');
    await tryMkdir(windowsDir);

    const replacements = {
      name: 'WithWinUI3',
      namespace: 'WithWinUI3',
      useMustache: true,
      projectGuidUpper: testProjectGuid,
      projectGuidLower: testProjectGuid.toLowerCase(),
      useWinUI3: false,
      useHermes: false,
      packagesConfigCppNugetPackages: [
        {
          id: 'Microsoft.ReactNative.Cxx',
          version: '1.0.0',
          hasProps: false,
          hasTargets: true,
        },
        {
          id: 'Microsoft.UI.Xaml',
          version: '2.3.4.5',
          hasProps: false, // WinUI/MUX props and targets get handled by RNW's WinUI.props.
          hasTargets: false,
        },
        {
          id: 'Microsoft.WinUI',
          version: '3.2.1.0',
          hasProps: false, // WinUI/MUX props and targets get handled by RNW's WinUI.props.
          hasTargets: false,
        },
      ],
    };

    await copyAndReplace(
      path.join(templateRoot, 'cpp-app/proj/MyApp.sln'),
      path.join(windowsDir, 'WithWinUI3.sln'),
      replacements,
      null,
    );

    const projDir = path.join(windowsDir, 'WithWinUI3');
    await tryMkdir(projDir);

    await copyAndReplace(
      path.join(templateRoot, 'cpp-app/proj/MyApp.vcxproj'),
      path.join(projDir, 'WithWinUI3.vcxproj'),
      replacements,
      null,
    );

    await copyAndReplace(
      path.join(templateRoot, 'cpp-app/proj/packages.config'),
      path.join(projDir, 'packages.config'),
      replacements,
      null,
    );

    await copyAndReplace(
      path.join(templateRoot, 'shared-app/proj/BuildFlags.props'),
      path.join(windowsDir, 'BuildFlags.props'),
      replacements,
      null,
    );
  }),
];

// Tests that given userConfig is null, the result will always be null
test.each(projects)(
  'projectConfig - %s (userConfig is null)',
  async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);

    if (setup !== undefined) {
      await setup(folder);
    }

    const userConfig = null;
    const expectedConfig: WindowsProjectConfig | null = null;

    expect(projectConfigWindows(folder, userConfig)).toBe(expectedConfig);
  },
);

// Tests the result given a windows project config in react-native.config.js
test.each(projects)(
  'projectConfig - %s (Use react-native.config.js)',
  async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);
    const rnc = require(path.join(folder, 'react-native.config.js'));

    if (setup !== undefined) {
      await setup(folder);
    }

    const userConfig: Partial<WindowsProjectConfig> = rnc.project.windows;
    const expectedConfig: WindowsProjectConfig | null = rnc.expectedConfig;

    if (expectedConfig !== null) {
      expectedConfig.folder = folder;
    }

    expect(projectConfigWindows(folder, userConfig)).toEqual(expectedConfig);
  },
);

// Tests the result of ignoring the windows project config in react-native.config.js
test.each(projects)(
  'projectConfig - %s (Ignore react-native.config.js)',
  async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);
    const rnc = require(path.join(folder, 'react-native.config.js'));

    if (setup !== undefined) {
      await setup(folder);
    }

    const userConfig: Partial<WindowsProjectConfig> = {};
    const expectedConfig: WindowsProjectConfig | null =
      rnc.expectedConfigIgnoringOverride;

    if (expectedConfig !== null) {
      expectedConfig.folder = folder;
    }

    expect(projectConfigWindows(folder, userConfig)).toEqual(expectedConfig);
  },
);

test('useWinUI3=true in react-native.config.js, useWinUI3=false in BuildFlags.props', async done => {
  const folder = path.resolve('src/e2etest/projects/WithWinUI3');
  const rnc = require(path.join(folder, 'react-native.config.js'));

  const config = projectConfigWindows(folder, rnc.project.windows);

  expect(ensureWinUIDialect(config!, false)).toBeTruthy();

  const packagesConfig = (
    await fs.promises.readFile(
      path.join(folder, 'windows/WithWinUI3/packages.config'),
    )
  ).toString();

  const buildFlags = (
    await fs.promises.readFile(path.join(folder, 'windows/BuildFlags.props'))
  ).toString();

  expect(packagesConfig.replace(/\r/g, '')).toEqual(rnc.expectedPackagesConfig);

  expect(buildFlags.replace(/\r/g, '')).toEqual(rnc.expectedBuildFlags);

  done();
});
