/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import fs from '@react-native-windows/fs';
import path from 'path';

import {
  copyProjectTemplateAndReplace,
  installScriptsAndDependencies,
} from './generator-windows';

import * as pathHelpers from './utils/pathHelpers';

import {autolinkCommand} from './commands/autolinkWindows/autolinkWindows';
import {codegenCommand} from './commands/codegenWindows/codegenWindows';
import {initCommand} from './commands/initWindows/initWindows';
import {runWindowsCommand} from './commands/runWindows/runWindows';
import {
  setupModuleWindowsCommand,
  setupModuleWindowsInternal,
} from './commands/setupModuleWindows/setupModuleWindows';
import {dependencyConfigWindows} from './commands/config/dependencyConfig';
import {projectConfigWindows} from './commands/config/projectConfig';

/**
 * Project generation options
 *
 *      _
 *     | |
 *   __| | __ _ _ __   __ _  ___ _ __
 *  / _` |/ _` | '_ \ / _` |/ _ \ '__|
 * | (_| | (_| | | | | (_| |  __/ |
 *  \__,_|\__,_|_| |_|\__, |\___|_|
 *                     __/ |
 *                    |___/
 *
 *
 * Properties on this interface must remain stable, as new versions of
 * react-native-windows-init may be used with local-cli dating back to 0.61.
 * All existing arguments must work correctly when changing this interface.
 */
export interface GenerateOptions {
  overwrite: boolean;
  language: 'cpp' | 'cs';
  projectType: 'app' | 'lib';
  experimentalNuGetDependency: boolean;
  nuGetTestVersion?: string;
  nuGetTestFeed?: string;
  useWinUI3: boolean;
  useHermes: boolean;
  useDevMode: boolean;
  verbose: boolean;
  telemetry: boolean;
}

/**
 * Simple utility for running the Windows generator.
 *
 * @param  projectDir root project directory (i.e. contains index.js)
 * @param  name       name of the root JS module for this app
 * @param  ns         namespace for the project
 * @param  options    command line options container
 */
export async function generateWindows(
  projectDir: string,
  name: string,
  ns: string,
  options: GenerateOptions,
) {
  if (!fs.existsSync(projectDir)) {
    fs.mkdirSync(projectDir);
  }

  await installScriptsAndDependencies(options);

  const rnwPackage = pathHelpers.resolveRnwRoot(projectDir);
  const templateRoot = path.join(rnwPackage, 'template');
  await copyProjectTemplateAndReplace(
    templateRoot,
    projectDir,
    name,
    ns,
    options,
  );
}

// Assert the interface here doesn't change for the reasons above
const assertStableInterface: typeof generateWindows extends (
  projectDir: string,
  name: string,
  ns: string,
  options: GenerateOptions,
) => Promise<void>
  ? true
  : never = true;
assertStableInterface;

export const commands = [
  autolinkCommand,
  codegenCommand,
  initCommand,
  runWindowsCommand,
  setupModuleWindowsCommand,
];
export const dependencyConfig = dependencyConfigWindows;
export const projectConfig = projectConfigWindows;
export {setupModuleWindowsInternal} from './commands/setupModuleWindows/setupModuleWindows';
export * from './commands/healthCheck/healthChecks';
