/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';

import {NpmPackagesWeTrack} from '../telemetry';

const nodeModules = '\\node_modules\\';
const windows = '\\windows\\';

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
  filepath: string | undefined,
  projectRoot?: string,
): string {
  if (filepath === undefined) {
    return '[path]';
  }

  projectRoot = (projectRoot ?? process.cwd())
    .replace(/\//g, '\\')
    .toLowerCase();
  projectRoot = projectRoot.endsWith('\\')
    ? projectRoot.slice(0, -1)
    : projectRoot;
  filepath = filepath.replace(/\//g, '\\');

  const ext = path.extname(filepath);

  // Check if we're under node_modules
  const nodeModulesIndex = filepath.toLowerCase().lastIndexOf(nodeModules);
  if (nodeModulesIndex >= 0) {
    // We are under node_modules

    // Check if it's an npm package we're tracking
    for (const trackedNpmPackage of NpmPackagesWeTrack) {
      const startIndex = filepath
        .toLowerCase()
        .lastIndexOf(
          nodeModules + trackedNpmPackage.replace(/\//g, '\\') + '\\',
        );
      if (startIndex >= 0) {
        // We are under node_modules within an npm package we're tracking, anonymize by removing root
        return (
          '[node_modules]\\' + filepath.slice(startIndex + nodeModules.length)
        );
      }
    }

    // It's an npm package we're not tracking, anonymize with [node_modules]
    return `[node_modules]\\???${ext}(${
      filepath.slice(nodeModulesIndex).length - nodeModules.length
    })`;
  }

  // Check if we're under the projectRoot
  if (filepath.toLowerCase().startsWith(projectRoot)) {
    // We are under the projectRoot
    const rest = filepath.slice(projectRoot.length);
    if (rest.toLowerCase().startsWith(windows)) {
      // We are under the windows path, anonymize with [windows]
      return `[windows]\\???${ext}(${rest.length - windows.length})`;
    } else {
      // We are just within the projectRoot, anonymize with [project_dir]
      if (rest === '' || rest === '\\') {
        return '[project_dir]';
      } else {
        return `[project_dir]\\???${ext}(${
          rest.length - (rest.startsWith('\\') ? 1 : 0)
        })`;
      }
    }
  }

  // Check if we're under a known environmental variable path
  for (const knownPath of knownEnvironmentVariablePaths) {
    if (
      process.env[knownPath] &&
      filepath.toLowerCase().startsWith(process.env[knownPath]!.toLowerCase())
    ) {
      return `[${knownPath}]\\???(${
        filepath.length - process.env[knownPath]!.length
      })`;
    }
  }

  // We are somewhere else, anonymize with [path]
  return '[path]';
}
