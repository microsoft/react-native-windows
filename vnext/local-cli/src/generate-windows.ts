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

/**
 * Simple utility for running the Windows generator.
 *
 * @param  projectDir root project directory (i.e. contains index.js)
 * @param  name       name of the root JS module for this app
 * @param  ns         namespace for the project
 * @param  {Object} options    command line options container
 */
export default function generateWindows(
  projectDir: string,
  name: string,
  ns: string,
  options,
) {
  if (!fs.existsSync(projectDir)) {
    fs.mkdirSync(projectDir);
  }

  installDependencies(options);

  copyProjectTemplateAndReplace(
    path.join(__dirname, '..', 'generator-windows', 'templates'),
    projectDir,
    name,
    {
      ns,
      overwrite: options.overwrite,
      language: options.language,
      experimentalNuGetDependency: options.experimentalNuGetDependency,
      useWinUI3: options.useWinUI3,
    },
  );
}
