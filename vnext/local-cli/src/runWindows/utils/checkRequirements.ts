/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {execSync} from 'child_process';
import * as path from 'path';
import * as shell from 'shelljs';
import Version from './version';

type Requirement =
  | 'os'
  | 'msbuild'
  | 'visualstudio'
  | 'windowssdk'
  | 'phonesdk';

const REQUIRED_VERSIONS: Record<string, Record<Requirement, string>> = {
  '10.0': {
    os: '6.3',
    msbuild: '14.0',
    visualstudio: '14.0',
    windowssdk: '10.0',
    phonesdk: '10.0',
  },
};

function shortenVersion(version: Version): string {
  return /^(\d+(?:\.\d+)?)/.exec(version.toString())[1];
}

function getMinimalRequiredVersionFor(
  requirement: Requirement,
  windowsTargetVersion: keyof (typeof REQUIRED_VERSIONS),
): Version | null {
  return Version.tryParse(REQUIRED_VERSIONS[windowsTargetVersion][requirement]);
}

function getInstalledWindowsSdks(): Version[] {
  const installedSdks: Version[] = [];

  const execString =
    'reg query "HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows" /s /v InstallationFolder /reg:32';
  let output: string;
  try {
    output = execSync(execString).toString();
  } catch (e) {
    return installedSdks;
  }

  const re = /\\Microsoft SDKs\\Windows\\v(\d+\.\d+)\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
  let match: RegExpExecArray;
  while ((match = re.exec(output))) {
    const sdkPath = match[2];
    if (shell.test('-e', path.join(sdkPath, 'SDKManifest.xml'))) {
      installedSdks.push(Version.tryParse(match[1]));
    }
  }

  return installedSdks;
}

function checkWinSdk(windowsTargetVersion: string): string {
  const installedSdks = getInstalledWindowsSdks();
  const requiredVersion = getMinimalRequiredVersionFor(
    'windowssdk',
    windowsTargetVersion,
  );
  const hasSdkInstalled = installedSdks.some(installedSdk =>
    installedSdk.eq(requiredVersion),
  );

  if (hasSdkInstalled) {
    return shortenVersion(requiredVersion);
  }

  if (!hasSdkInstalled) {
    const shortenedVersion = shortenVersion(requiredVersion);
    throw new Error(
      `Windows SDK not found. Ensure that you have installed Windows ${shortenedVersion} SDK along with Visual Studio or install Windows ${shortenedVersion} SDK separately from https://dev.windows.com/en-us/downloads`,
    );
  }
}

export function isWinSdkPresent(target: string): string {
  return checkWinSdk(target);
}
