/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';
import {
  copyProjectTemplateAndReplace,
  installDependencies,
} from './generator-windows';

import {autoLinkCommand} from './runWindows/utils/autolink';
import {runWindowsCommand} from './runWindows/runWindows';
import {dependencyConfigWindows} from './config/dependencyConfig';
import {projectConfigWindows} from './config/projectConfig';

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
  verbose: boolean;
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

  installDependencies(options);

  const rnwPackage = path.dirname(
    require.resolve('react-native-windows/package.json', {paths: [projectDir]}),
  );
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

export const commands = [autoLinkCommand, runWindowsCommand];
export const dependencyConfig = dependencyConfigWindows;
export const projectConfig = projectConfigWindows;
