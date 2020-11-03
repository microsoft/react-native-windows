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

const templateRoot = path.resolve('../../../vnext/template');

const testProjectGuid = '{416476D5-974A-4EE2-8145-4E331297247E}';

function projectTests(testAppName: string, setup?: Function): void {
  const folder = path.resolve('src/test/projects/', testAppName);
  const rnc = require(path.join(folder, 'react-native.config.js'));

  if (setup !== undefined) {
    setup(folder);
  }

  test(`projectConfig - ${testAppName} (userConfig is null)`, () => {
    // Tests that given userConfig is null, the result will alwyas be null
    const userConfig = null;
    const expectedConfig: WindowsProjectConfig | null = null;

    expect(projectConfigWindows(folder, userConfig)).toBe(expectedConfig);
  });

  test(`projectConfig - ${testAppName} (Use react-native.config.js)`, () => {
    // Tests the result given a windows project config in react-native.config.js
    const userConfig: Partial<WindowsProjectConfig> = rnc.project.windows;
    let expectedConfig: WindowsProjectConfig | null = rnc.expectedConfig;

    if (expectedConfig !== null) {
      expectedConfig.folder = folder;
    }

    expect(projectConfigWindows(folder, userConfig)).toEqual(expectedConfig);
  });

  test(`projectConfig - ${testAppName} (Ignore react-native.config.js)`, () => {
    // Tests the result of ignoring the windows project config in react-native.config.js
    const userConfig: Partial<WindowsProjectConfig> = {};
    const expectedConfig: WindowsProjectConfig | null =
      rnc.expectedConfigIgnoringOverride;

    if (expectedConfig !== null) {
      expectedConfig.folder = folder;
    }

    expect(projectConfigWindows(folder, userConfig)).toEqual(expectedConfig);
  });
}

// Nothing but a react-native.config.js, with nulls
projectTests('BlankApp');

// Nothing but a windows folder
projectTests('MissingProjectFilesApp', (folder: string) => {
  const windowsDir = path.join(folder, 'windows');
  if (!fs.existsSync(windowsDir)) {
    fs.mkdirSync(windowsDir);
  }
});

// New C++ project based on the template
projectTests('SimpleCppApp', (folder: string) => {
  const windowsDir = path.join(folder, 'windows');
  if (!fs.existsSync(windowsDir)) {
    fs.mkdirSync(windowsDir);
  }

  const replacements = {
    name: 'SimpleCppApp',
    namespace: 'SimpleCppApp',
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
  };

  void copyAndReplace(
    path.join(templateRoot, 'cpp-app/proj/MyApp.sln'),
    path.join(windowsDir, 'SimpleCppApp.sln'),
    replacements,
    null,
  );

  const projDir = path.join(windowsDir, 'SimpleCppApp');
  if (!fs.existsSync(projDir)) {
    fs.mkdirSync(projDir);
  }
  void copyAndReplace(
    path.join(templateRoot, 'cpp-app/proj/MyApp.vcxproj'),
    path.join(projDir, 'SimpleCppApp.vcxproj'),
    replacements,
    null,
  );
});

// New C# project based on the template
projectTests('SimpleCSharpApp', (folder: string) => {
  const windowsDir = path.join(folder, 'windows');
  if (!fs.existsSync(windowsDir)) {
    fs.mkdirSync(windowsDir);
  }

  const replacements = {
    name: 'SimpleCSharpApp',
    namespace: 'SimpleCSharpApp',
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
  };

  void copyAndReplace(
    path.join(templateRoot, 'cs-app/proj/MyApp.sln'),
    path.join(windowsDir, 'SimpleCSharpApp.sln'),
    replacements,
    null,
  );

  const projDir = path.join(windowsDir, 'SimpleCSharpApp');
  if (!fs.existsSync(projDir)) {
    fs.mkdirSync(projDir);
  }
  void copyAndReplace(
    path.join(templateRoot, 'cs-app/proj/MyApp.csproj'),
    path.join(projDir, 'SimpleCSharpApp.csproj'),
    replacements,
    null,
  );
});
