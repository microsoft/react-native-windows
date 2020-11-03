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

async function tryMkdir(dir: string): Promise<void> {
  try {
    await fs.promises.mkdir(dir, {recursive: true});
  } catch (err) {}
}

function projectTests(
  testAppName: string,
  setup?: (folder: string) => Promise<void>,
): void {
  const folder = path.resolve('src/test/projects/', testAppName);
  const rnc = require(path.join(folder, 'react-native.config.js'));

  test(`projectConfig - ${testAppName} (userConfig is null)`, async () => {
    if (setup !== undefined) {
      await setup(folder);
    }

    // Tests that given userConfig is null, the result will always be null
    const userConfig = null;
    const expectedConfig: WindowsProjectConfig | null = null;

    expect(projectConfigWindows(folder, userConfig)).toBe(expectedConfig);
  });

  test(`projectConfig - ${testAppName} (Use react-native.config.js)`, async () => {
    if (setup !== undefined) {
      await setup(folder);
    }

    // Tests the result given a windows project config in react-native.config.js
    const userConfig: Partial<WindowsProjectConfig> = rnc.project.windows;
    let expectedConfig: WindowsProjectConfig | null = rnc.expectedConfig;

    if (expectedConfig !== null) {
      expectedConfig.folder = folder;
    }

    expect(projectConfigWindows(folder, userConfig)).toEqual(expectedConfig);
  });

  test(`projectConfig - ${testAppName} (Ignore react-native.config.js)`, async () => {
    if (setup !== undefined) {
      await setup(folder);
    }

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
projectTests('MissingProjectFilesApp', async (folder: string) => {
  const windowsDir = path.join(folder, 'windows');
  await tryMkdir(windowsDir);
});

// New C++ project based on the template
projectTests('SimpleCppApp', async (folder: string) => {
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
});

// New C# project based on the template
projectTests('SimpleCSharpApp', async (folder: string) => {
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
});
