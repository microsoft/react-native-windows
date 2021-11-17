/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';

import {NpmPackagesWeTrack} from '../telemetry';

const nodeModules = '\\node_modules\\';

const knownEnvironmentVariablePaths = [
  'AppData',
  'LocalAppData',
  'UserProfile',
];

/**
 * Gets an anonymized version of the given path, suitable for Telemetry.
 * @param filepath The path to anonymize.
 * @param projectRoot Optional root path for the project. Defaults to process.cwd().
 * @returns The anonymized path.
 */
export function getAnonymizedPath(
  filepath: string,
  projectRoot?: string,
): string {
  projectRoot = (projectRoot ?? process.cwd())
    .replace(/\//g, '\\')
    .toLowerCase();
  filepath = filepath.replace(/\//g, '\\');

  if (filepath.toLowerCase().startsWith(projectRoot)) {
    // We are under the projectRoot
    const ext = path.extname(filepath);
    const rest = filepath.slice(projectRoot.length);
    if (rest.toLowerCase().startsWith('\\windows\\')) {
      // We are under the windows path, anonymize with [windows]
      return `[windows]\\???${ext}(${filepath.length})`;
    } else if (rest.toLowerCase().startsWith(nodeModules)) {
      // We are under the node_modules path
      for (const trackedNpmPackage of NpmPackagesWeTrack) {
        if (
          rest
            .toLowerCase()
            .startsWith(
              nodeModules + trackedNpmPackage.replace(/\//g, '\\') + '\\',
            )
        ) {
          // We are under node_modules within an npm package we're tracking, anonymize by removing root
          return '[node_modules]' + rest.slice(nodeModules.length - 1);
        }
      }
      // We are under node_modules within an npm package we're not tracking, anonymize with [node_modules]
      return `[node_modules]\\???${ext}(${filepath.length})`;
    } else {
      // We are just within the projectRoot, anonymize with [project_dir]
      return `[project_dir]\\???${ext}(${filepath.length})`;
    }
  } else {
    for (const knownPath of knownEnvironmentVariablePaths) {
      if (
        process.env[knownPath] &&
        filepath.toLowerCase().startsWith(process.env[knownPath]!.toLowerCase())
      ) {
        return `[${knownPath}]\\???(${filepath.length})`;
      }
    }
  }
  // We are somewhere else, anonymize with [path]
  return '[path]';
}
