/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * This file contains some code taken from the doxygen project.
 * Copyright for the doxygen package code:
 * Copyright (c) 2016 Ivan Maria <iv.ivan.maria@gmail.com>;
 *
 * @format
 */

/**
 * Generates Doxygen documentation XML files.
 */

// @ts-ignore (no typings for doxygen)
import constants from 'doxygen/lib/constants';
// @ts-ignore (no typings for doxygen)
import doxygen from 'doxygen';
import path from 'path';
import {exec} from 'child_process';
import {log} from './logger';
import {Config} from './config';

const DOXYGEN_VERSION = '1.9.1';

export async function generateDoxygenXml(config: Config): Promise<void> {
  const doxygenConfigPath = path.join(config.buildDir, 'doxygen.config');
  generateDoxygenConfig(config, doxygenConfigPath);

  if (!doxygen.isDoxygenExecutableInstalled(DOXYGEN_VERSION)) {
    log(`[Downloading] Doxygen version {${DOXYGEN_VERSION}} ...`);
    await doxygen.downloadVersion(DOXYGEN_VERSION);
    log(`[Downloaded] Doxygen version {${DOXYGEN_VERSION}}`);
  }

  const {stdout, stderr} = await runAsync(doxygenConfigPath, DOXYGEN_VERSION);

  for (const info of stdout.split('\n')) {
    log(`[Doxygen:] ${info}`);
  }

  // Doxygen process reports all warnings in the stderr stream.
  for (const warning of stderr.split('\n')) {
    log.warning(warning);
  }
}

function generateDoxygenConfig(
  config: Config,
  doxygenConfigPath: string,
): void {
  const doxygenOptions: {[index: string]: string} = {
    OUTPUT_DIRECTORY: config.buildDir,
    INPUT: config.input,
    GENERATE_LATEX: 'NO',
    GENERATE_HTML: 'NO',
    GENERATE_XML: 'YES',
  };

  if (config.filePatterns) {
    doxygenOptions.FILE_PATTERNS = config.filePatterns.join(' ');
  }

  doxygen.createConfig(doxygenOptions, doxygenConfigPath);
}

// Modified from doxygen NPM.
// This is a not exported method that we need for the runAsync implementation.
// The code is modified for TypeScript, eslint, and the different __dirname.
function doxygenExecutablePath(version?: string): string {
  // const dirName = __dirname; -- we must use the doxygen package path
  const dirName = path.dirname(require.resolve('doxygen/package.json'));

  const doxygenFolder =
    process.platform === constants.platform.macOS.identifier
      ? constants.path.macOsDoxygenFolder
      : '';

  const ext =
    process.platform === constants.platform.windows.identifier ? '.exe' : '';
  return path.normalize(
    path.join(
      dirName,
      'dist',
      version || constants.default.version,
      doxygenFolder,
      'doxygen' + ext,
    ),
  );
}

// Modified from doxygen NPM.
// Extends doxygen.run to return a Promise.
// We cannot use util.promisify because it loses the stderr with warnings.
async function runAsync(
  configPath?: string,
  version?: string,
): Promise<{stdout: string; stderr: string}> {
  const doxygenPath = doxygenExecutablePath(version);
  return new Promise((resolve, reject) => {
    exec(
      `"${doxygenPath}" "${path.resolve(
        configPath || constants.path.configFile,
      )}"`,
      (error, stdout, stderr) => {
        if (error) {
          reject(error);
        } else {
          resolve({stdout, stderr});
        }
      },
    );
  });
}
