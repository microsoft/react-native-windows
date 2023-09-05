/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import fs from '@react-native-windows/fs';
import path from 'path';

import {
  projectConfigWindows,
  WindowsProjectConfig,
} from '../commands/config/projectConfig';

import {AutoLinkWindows} from '../commands/autolinkWindows/autolinkWindows';
import {
  ensureCppAppProject,
  ensureCSharpAppProject,
  ensureWinUI3Project,
  ensureHermesProject,
  ensureExperimentalNuGetProject,
  tryMkdir,
} from './projectConfig.utils';

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
    await ensureCppAppProject(folder, 'SimpleCppApp');
  }),
  // New C# project based on the template
  project('SimpleCSharpApp', async (folder: string) => {
    await ensureCSharpAppProject(folder, 'SimpleCSharpApp');
  }),
  project('WithWinUI3', async (folder: string) => {
    await ensureWinUI3Project(folder);
  }),
  project('WithHermes', async (folder: string) => {
    await ensureHermesProject(folder);
  }),
  project('WithExperimentalNuget', async (folder: string) => {
    await ensureExperimentalNuGetProject(folder);
  }),
  project('WithIndirectDependency'),
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

    expect(projectConfigWindows(folder, userConfig)).toBeNull();
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

    if (name === 'BlankApp') {
      expect(projectConfigWindows(folder, userConfig)).toMatchSnapshot();
    } else {
      expect(projectConfigWindows(folder, userConfig)).toMatchSnapshot({
        folder: expect.stringContaining(name),
      });
    }
  },
);

// Tests the result of ignoring the windows project config in react-native.config.js
test.each(projects)(
  'projectConfig - %s (Ignore react-native.config.js)',
  async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);

    if (setup !== undefined) {
      await setup(folder);
    }

    const userConfig: Partial<WindowsProjectConfig> = {};

    if (name === 'BlankApp') {
      expect(projectConfigWindows(folder, userConfig)).toMatchSnapshot();
    } else {
      expect(projectConfigWindows(folder, userConfig)).toMatchSnapshot({
        folder: expect.stringContaining(name),
      });
    }
  },
);

test('useWinUI3=true in react-native.config.js, UseWinUI3=false in ExperimentalFeatures.props', async () => {
  const folder = path.resolve('src/e2etest/projects/WithWinUI3');

  // Create project with UseWinUI3 == false in ExperimentalFeatures.props
  await ensureCppAppProject(folder, 'WithWinUI3', false, false, false);

  const rnc = require(path.join(folder, 'react-native.config.js'));

  const config = projectConfigWindows(folder, rnc.project.windows)!;
  // Set useWinUI3=true in react-native.config.js
  config.useWinUI3 = true;

  const experimentalFeaturesPropsFile = path.join(
    folder,
    'windows/ExperimentalFeatures.props',
  );

  // Verify starting props file
  const startingExperimentalFeatures = (
    await fs.readFile(experimentalFeaturesPropsFile)
  ).toString();
  expect(startingExperimentalFeatures.replace(/\r/g, '')).toMatchSnapshot();

  // Run Autolink to sync the files
  const al = new AutoLinkWindows(
    {windows: config},
    {},
    {
      check: false,
      logging: false,
    },
  );

  const exd = await al.ensureXAMLDialect();
  expect(exd).toBeTruthy();

  // Verify ending props file
  const finalExperimentalFeatures = (
    await fs.readFile(experimentalFeaturesPropsFile)
  ).toString();

  expect(finalExperimentalFeatures.replace(/\r/g, '')).toMatchSnapshot();
});
