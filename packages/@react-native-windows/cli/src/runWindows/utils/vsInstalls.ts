/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {execSync} from 'child_process';
import * as fs from 'fs';
import * as path from 'path';

/**
 * A subset of the per-instance properties returned by vswhere
 */
interface VisualStudioInstallation {
  instanceId: string;
  installationPath: string;
  installationVersion: string;
}

/**
 * Helper to run vswhere in JSON mode
 *
 * @param args Arguments to pass to vsWhere
 * @param verbose enable verbose logging
 */
function vsWhere(args: string[], verbose?: boolean): any[] {
  // This path is maintained and VS has promised to keep it valid.
  const vsWherePath = path.join(
    process.env['ProgramFiles(x86)'] || process.env.ProgramFiles!,
    '/Microsoft Visual Studio/Installer/vswhere.exe',
  );

  if (verbose) {
    console.log('Looking for vswhere at: ' + vsWherePath);
  }

  if (!fs.existsSync(vsWherePath)) {
    throw new Error(`Unable to find vswhere at ${vsWherePath}`);
  }

  return JSON.parse(
    execSync(`"${vsWherePath}" ${args.join(' ')} -format json -utf8`).toString(
      'utf8',
    ),
  );
}

/**
 * Enumerate the installed versions of Visual Studio
 */
export function enumerateVsInstalls(opts: {
  requires?: string[];
  version?: string;
  verbose?: boolean;
  latest?: boolean;
}): VisualStudioInstallation[] {
  const args: string[] = [];

  if (opts.version) {
    args.push(`-version [${opts.version},${Number(opts.version) + 1})`);
  }

  if (opts.requires) {
    args.push(`-requires ${opts.requires.join(' ')}`);
  }

  if (opts.latest) {
    args.push('-latest');
  }

  return vsWhere(args, opts.verbose);
}

/**
 * Find the latest available VS installation that matches the given constraints
 */
export function findLatestVsInstall(opts: {
  requires?: string[];
  version?: string;
  verbose?: boolean;
}): VisualStudioInstallation | null {
  const installs = enumerateVsInstalls({...opts, latest: true});
  if (installs.length > 0) {
    return installs[0];
  } else {
    return null;
  }
}
