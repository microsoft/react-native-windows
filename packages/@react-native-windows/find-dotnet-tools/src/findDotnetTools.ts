/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {execSync} from 'child_process';
import fs from '@react-native-windows/fs';
import os from 'os';
import path from 'path';

/**
 * Returns the path to the global NuGet packages folder, checking (in order):
 * 1. The NUGET_PACKAGES environment variable
 * 2. The `dotnet nuget locals` command output
 * 3. The default ~/.nuget/packages location
 */
export function getNugetGlobalPackagesFolder(): string {
  if (process.env.NUGET_PACKAGES) {
    return process.env.NUGET_PACKAGES;
  }
  try {
    const output = execSync('dotnet.exe nuget locals global-packages --list', {
      encoding: 'utf8',
    }).trim();
    const match = output.match(/global-packages:\s*(.+)/i);
    if (match) {
      return match[1].trim();
    }
  } catch {}
  return path.join(os.homedir(), '.nuget', 'packages');
}

/**
 * Locates a PowerShell executable, checking (in order):
 * 1. A NuGet-restored copy of pwsh (skipped in CI builds)
 * 2. pwsh.exe on the system PATH
 * 3. The built-in Windows PowerShell as a last resort
 */
export function findPowerShell(): string {
  // Build agents already have PowerShell (pwsh) installed
  if (!process.env.TF_BUILD) {
    const nugetPackages = getNugetGlobalPackagesFolder();
    const nugetPwsh = path.join(
      nugetPackages,
      'PowerShell',
      '7.6.1',
      'tools',
      'net10.0',
      'any',
      'win',
      'pwsh.exe',
    );
    if (fs.existsSync(nugetPwsh)) {
      return nugetPwsh;
    }
  }

  try {
    const found = execSync('where pwsh.exe', {encoding: 'utf8'}).trim();
    if (found) {
      return found.split(/\r?\n/)[0];
    }
  } catch {}

  return `${process.env.SystemRoot}\\System32\\WindowsPowerShell\\v1.0\\powershell.exe`;
}
