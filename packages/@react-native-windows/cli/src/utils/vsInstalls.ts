/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {CodedError} from '@react-native-windows/telemetry';
import {execSync} from 'child_process';
import fs from '@react-native-windows/fs';
import path from 'path';
import semver from 'semver';

/**
 * A subset of the per-instance properties returned by vswhere
 */
interface VisualStudioInstallation {
  instanceId: string;
  installationPath: string;
  installationVersion: string;
  prerelease: string;
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
    throw new CodedError(
      'NoVSWhere',
      `Unable to find vswhere at ${vsWherePath}`,
    );
  }

  const system32 = `${process.env.SystemRoot}\\System32`;
  const cmdline = `${system32}\\cmd.exe /c ${system32}\\chcp.com 65001>nul && "${vsWherePath}" ${args.join(
    ' ',
  )} -format json -utf8`;
  const json = JSON.parse(execSync(cmdline).toString());

  for (const entry of json) {
    entry.prerelease = entry.catalog.productMilestoneIsPreRelease;
  }

  return json;
}

/**
 * Enumerate the installed versions of Visual Studio
 */
export function enumerateVsInstalls(opts: {
  requires?: string[];
  minVersion?: string;
  verbose?: boolean;
  latest?: boolean;
  prerelease?: boolean;
}): VisualStudioInstallation[] {
  const args: string[] = [];

  if (opts.minVersion) {
    // VS 2019 ex: minVersion == 16.7 => [16.7,17.0)
    // VS 2022 ex: minVersion == 17.0 => [17.0,18.0)

    // Try to parse minVersion as both a Number and SemVer
    const minVersionNum = Number(opts.minVersion);
    const minVersionSemVer = semver.parse(opts.minVersion);

    let minVersion: string;
    let maxVersion: string;

    if (minVersionSemVer) {
      minVersion = minVersionSemVer.toString();
      maxVersion = `${minVersionSemVer.major + 1}.0`;
    } else if (!Number.isNaN(minVersionNum)) {
      minVersion = Number.isInteger(minVersionNum)
        ? `${minVersionNum}.0`
        : minVersionNum.toString();
      maxVersion = `${Math.floor(minVersionNum) + 1}.0`;
    } else {
      // Unable to parse minVersion and determine maxVersion,
      // caller will throw error that version couldn't be found.
      return [];
    }

    const versionRange =
      `[${minVersion},${maxVersion}` + (opts.prerelease ? ']' : ')');

    if (opts.verbose) {
      console.log(
        `Looking for VS installs with version range: ${versionRange}`,
      );
    }

    args.push(`-version ${versionRange}`);
  }

  if (opts.requires) {
    args.push(`-requires ${opts.requires.join(' ')}`);
  }

  if (opts.latest) {
    args.push('-latest');
  }

  if (opts.prerelease) {
    args.push('-prerelease');
  }

  return vsWhere(args, opts.verbose);
}

/**
 * Find the latest available VS installation that matches the given constraints
 */
export function findLatestVsInstall(opts: {
  requires?: string[];
  minVersion?: string;
  verbose?: boolean;
  prerelease?: boolean;
}): VisualStudioInstallation | null {
  let installs = enumerateVsInstalls({...opts, latest: true});

  if (opts.prerelease && installs.length > 0) {
    installs = installs.filter(x => x.prerelease === 'True');
  }

  if (installs.length > 0) {
    return installs[0];
  } else {
    return null;
  }
}
