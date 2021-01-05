/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as path from 'path';

import MSBuildTools from './msbuildtools';
import Version from './version';
import {newError} from './commandWithProgress';
import {RunWindowsOptions, BuildConfig, BuildArch} from '../runWindowsOptions';
import {Config} from '@react-native-community/cli-types';
import {CodedError} from '@react-native-windows/telemetry';

export async function buildSolution(
  buildTools: MSBuildTools,
  slnFile: string,
  buildType: BuildConfig,
  buildArch: BuildArch,
  msBuildProps: Record<string, string>,
  verbose: boolean,
  target: 'build' | 'deploy',
  buildLogDirectory?: string,
  singleproc?: boolean,
) {
  const minVersion = new Version(10, 0, 18362, 0);
  const allVersions = MSBuildTools.getAllAvailableUAPVersions();
  if (!allVersions.some(v => v.gte(minVersion))) {
    throw new CodedError(
      'MinSDKVersionNotMet',
      'Must have a minimum Windows SDK version 10.0.18362.0 installed',
    );
  }

  await buildTools.buildProject(
    slnFile,
    buildType,
    buildArch,
    msBuildProps,
    verbose,
    target,
    buildLogDirectory,
    singleproc,
  );
}

const configErrorString = 'Error: ';

export function getAppSolutionFile(options: RunWindowsOptions, config: Config) {
  // Use the solution file if specified
  if (options.sln) {
    return path.join(options.root, options.sln);
  }

  // Check the answer from react-native config
  const windowsAppConfig = config.project.windows;
  if (!windowsAppConfig) {
    throw new CodedError(
      'NoWindowsConfig',
      "Couldn't determine Windows app config",
    );
  }
  const configSolutionFile = windowsAppConfig.solutionFile;

  if (configSolutionFile.startsWith(configErrorString)) {
    newError(
      configSolutionFile.substr(configErrorString.length) +
        ' Optionally, use --sln {slnFile}.',
    );
    return null;
  } else {
    return path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      configSolutionFile,
    );
  }
}

export function getAppProjectFile(options: RunWindowsOptions, config: Config) {
  // Use the project file if specified
  if (options.proj) {
    return path.join(options.root, options.proj);
  }

  // Check the answer from react-native config
  const windowsAppConfig = config.project.windows;
  const configProject = windowsAppConfig.project;

  if (
    typeof configProject === 'string' &&
    configProject.startsWith(configErrorString)
  ) {
    newError(
      configProject.substr(configErrorString.length) +
        ' Optionally, use --proj {projFile}.',
    );
    return null;
  } else {
    const configProjectFile = configProject.projectFile;
    if (configProjectFile.startsWith(configErrorString)) {
      newError(
        configProjectFile.substr(configErrorString.length) +
          ' Optionally, use --proj {projFile}.',
      );
      return null;
    }
    return path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      configProjectFile,
    );
  }
}

export function parseMsBuildProps(
  options: RunWindowsOptions,
): Record<string, string> {
  const result: Record<string, string> = {};
  if (options.msbuildprops) {
    const props = options.msbuildprops.split(',');
    for (const prop of props) {
      const propAssignment = prop.split('=');
      result[propAssignment[0]] = propAssignment[1];
    }
  }
  return result;
}
