/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';

import {
  dependencyConfigWindows,
  WindowsDependencyConfig,
} from '../config/dependencyConfig';

import {copyAndReplace} from '../generator-common';

const templateRoot = path.resolve('../../../vnext/template');

const testProjectGuid = '{416476D5-974A-4EE2-8145-4E331297247E}';

function projectTests(testLibName: string, setup?: Function): void {
  const folder = path.resolve('src/test/projects/', testLibName);
  const rnc = require(path.join(folder, 'react-native.config.js'));

  if (setup !== undefined) {
    setup(folder);
  }

  test(`dependencyConfig - ${testLibName} (userConfig is null)`, () => {
    // Tests that given userConfig is null, the result will alwyas be null
    const userConfig = null;
    const expectedConfig: WindowsDependencyConfig | null = null;

    expect(dependencyConfigWindows(folder, userConfig)).toBe(expectedConfig);
  });

  test(`dependencyConfig - ${testLibName} (Use react-native.config.js)`, () => {
    // Tests the result given a windows project config in react-native.config.js
    const userConfig: Partial<WindowsDependencyConfig> = rnc.dependency.platforms.windows;
    let expectedConfig: WindowsDependencyConfig | null = rnc.expectedConfig;

    if (expectedConfig !== null) {
      expectedConfig.folder = folder;
    }

    expect(dependencyConfigWindows(folder, userConfig)).toEqual(expectedConfig);
  });

  test(`dependencyConfig - ${testLibName} (Ignore react-native.config.js)`, () => {
    // Tests the result of ignoring the windows project config in react-native.config.js
    const userConfig: Partial<WindowsDependencyConfig> = {};
    const expectedConfig: WindowsDependencyConfig | null =
      rnc.expectedConfigIgnoringOverride;

    if (expectedConfig !== null) {
      expectedConfig.folder = folder;
    }

    expect(dependencyConfigWindows(folder, userConfig)).toEqual(expectedConfig);
  });
}

// Nothing but a react-native.config.js, with nulls
projectTests('BlankLib');

// Nothing but a windows folder
projectTests('MissingProjectFilesLib', (folder: string) => {
  const windowsDir = path.join(folder, 'windows');
  if (!fs.existsSync(windowsDir)) {
    fs.mkdirSync(windowsDir);
  }
});

// New C++ project based on the template
projectTests('SimpleCppLib', (folder: string) => {
  const windowsDir = path.join(folder, 'windows');
  if (!fs.existsSync(windowsDir)) {
    fs.mkdirSync(windowsDir);
  }

  const replacements = {
    name: 'SimpleCppLib',
    namespace: 'SimpleCppLib',
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
  };

  void copyAndReplace(
    path.join(templateRoot, 'cpp-lib/proj/MyLib.sln'),
    path.join(windowsDir, 'SimpleCppLib.sln'),
    replacements,
    null,
  );

  const projDir = path.join(windowsDir, 'SimpleCppLib');
  if (!fs.existsSync(projDir)) {
    fs.mkdirSync(projDir);
  }
  void copyAndReplace(
    path.join(templateRoot, 'cpp-lib/proj/MyLib.vcxproj'),
    path.join(projDir, 'SimpleCppLib.vcxproj'),
    replacements,
    null,
  );
});

// New C# project based on the template
projectTests('SimpleCSharpLib', (folder: string) => {
  const windowsDir = path.join(folder, 'windows');
  if (!fs.existsSync(windowsDir)) {
    fs.mkdirSync(windowsDir);
  }

  const replacements = {
    name: 'SimpleCSharpLib',
    namespace: 'SimpleCSharpLib',
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
  };

  void copyAndReplace(
    path.join(templateRoot, 'cs-lib/proj/MyLib.sln'),
    path.join(windowsDir, 'SimpleCSharpLib.sln'),
    replacements,
    null,
  );

  const projDir = path.join(windowsDir, 'SimpleCSharpLib');
  if (!fs.existsSync(projDir)) {
    fs.mkdirSync(projDir);
  }
  void copyAndReplace(
    path.join(templateRoot, 'cs-lib/proj/MyLib.csproj'),
    path.join(projDir, 'SimpleCSharpLib.csproj'),
    replacements,
    null,
  );
});
