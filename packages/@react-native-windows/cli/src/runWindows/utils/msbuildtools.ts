/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {totalmem, EOL} from 'os';
import * as fs from 'fs';
import * as path from 'path';
import * as child_process from 'child_process';
import * as chalk from 'chalk';
import * as os from 'os';
import * as shell from 'shelljs';
import Version from './version';
import * as checkRequirements from './checkRequirements';
import {
  commandWithProgress,
  newInfo,
  newSpinner,
  newSuccess,
  newError,
} from './commandWithProgress';
import {execSync} from 'child_process';
import {BuildArch, BuildConfig} from '../runWindowsOptions';
import {findLatestVsInstall} from './vsInstalls';

export default class MSBuildTools {
  /**
   * @param version is something like 16.0 for 2019
   * @param msbuildPath  Path to MSBuild.exe (x86)
   * @param installationVersion is the full version e.g. 16.3.29411.108
   */
  constructor(
    public readonly version: string,
    public readonly msbuildPath: string,
    public readonly installationVersion: string,
  ) {}

  cleanProject(slnFile: string) {
    const cmd = `"${path.join(
      this.msbuildPath,
      'msbuild.exe',
    )}" "${slnFile}" /t:Clean`;
    const results = child_process
      .execSync(cmd)
      .toString()
      .split(EOL);
    results.forEach(result => console.log(chalk.white(result)));
  }

  async buildProject(
    slnFile: string,
    buildType: BuildConfig,
    buildArch: BuildArch,
    msBuildProps: Record<string, string>,
    verbose: boolean,
    target: string | undefined,
    buildLogDirectory: string | undefined,
    singleproc?: boolean,
  ) {
    newSuccess(`Found Solution: ${slnFile}`);
    newInfo(`Build configuration: ${buildType}`);
    newInfo(`Build platform: ${buildArch}`);
    if (target) {
      newInfo(`Build target: ${target}`);
    }
    const verbosityOption = verbose ? 'normal' : 'minimal';
    const logPrefix = path.join(
      buildLogDirectory || os.tmpdir(),
      `msbuild_${process.pid}${target ? '_' + target : ''}`,
    );

    const errorLog = logPrefix + '.err';
    const warnLog = logPrefix + '.wrn';

    const localBinLog = target ? `:${target}.binlog` : '';
    const binlog = buildLogDirectory ? `:${logPrefix}.binlog` : localBinLog;

    const args = [
      `/clp:NoSummary;NoItemAndPropertyList;Verbosity=${verbosityOption}`,
      '/nologo',
      `/p:Configuration=${buildType}`,
      `/p:Platform=${buildArch}`,
      '/p:AppxBundle=Never',
      `/bl${binlog}`,
      `/flp1:errorsonly;logfile=${errorLog}`,
      `/flp2:warningsonly;logfile=${warnLog}`,
    ];

    // Building projects in parallel increases compiler memory usage and
    // doesn't lead to dramatic performance gains (See #4739). Only enable
    // parallel builds on machines with >16GB of memory to avoid OOM errors
    const highMemory = totalmem() > 16 * 1024 * 1024 * 1024;
    const enableParallelBuilds = singleproc === false || highMemory;

    if (enableParallelBuilds) {
      args.push('/maxCpuCount');
    }

    if (target) {
      args.push(`/t:${target}`);
    }

    if (msBuildProps) {
      Object.keys(msBuildProps).forEach(function(key) {
        args.push(`/p:${key}=${msBuildProps[key]}`);
      });
    }

    try {
      checkRequirements.isWinSdkPresent('10.0');
    } catch (e) {
      newError(e.message);
      throw e;
    }

    if (verbose) {
      console.log(`Running MSBuild with args ${args.join(' ')}`);
    }

    const progressName = 'Building Solution';
    const spinner = newSpinner(progressName);
    try {
      await commandWithProgress(
        spinner,
        progressName,
        path.join(this.msbuildPath, 'msbuild.exe'),
        [slnFile].concat(args),
        verbose,
      );
    } catch (e) {
      let error = e;
      if (!e) {
        const firstMessage = (await fs.promises.readFile(errorLog))
          .toString()
          .split(EOL)[0];
        error = new Error(firstMessage);
        error.logfile = errorLog;
      }
      throw error;
    }
    // If we have no errors, delete the error log when we're done
    if ((await fs.promises.stat(errorLog)).size === 0) {
      await fs.promises.unlink(errorLog);
    }
  }

  static findAvailableVersion(
    buildArch: BuildArch,
    verbose: boolean,
    prerelease?: boolean,
  ): MSBuildTools {
    // https://aka.ms/vs/workloads
    const requires = [
      'Microsoft.Component.MSBuild',
      getVCToolsByArch(buildArch),
    ];
    const version = process.env.VisualStudioVersion || '16.0';
    const vsInstallation = findLatestVsInstall({
      requires,
      version,
      verbose,
      prerelease,
    });

    if (!vsInstallation) {
      if (process.env.VisualStudioVersion != null) {
        throw new Error(
          `MSBuild tools not found for version ${
            process.env.VisualStudioVersion
          } (from environment). Make sure all required components have been installed`,
        );
      } else {
        throw new Error(
          'MSBuild tools not found. Make sure all required components have been installed',
        );
      }
    }

    const toolsPath = path.join(
      vsInstallation.installationPath,
      'MSBuild/Current/Bin',
    );

    if (fs.existsSync(toolsPath)) {
      newSuccess(
        `Found MSBuild v${version} at ${toolsPath} (${
          vsInstallation.installationVersion
        })`,
      );
      return new MSBuildTools(
        version,
        toolsPath,
        vsInstallation.installationVersion,
      );
    } else {
      throw new Error(`MSBuild path '${toolsPath} does not exist'`);
    }
  }

  static getAllAvailableUAPVersions(): Version[] {
    const results: Version[] = [];

    const programFilesFolder =
      process.env['ProgramFiles(x86)'] || process.env.ProgramFiles;
    // No Program Files folder found, so we won't be able to find UAP SDK
    if (!programFilesFolder) {
      return results;
    }

    let uapFolderPath = path.join(
      programFilesFolder,
      'Windows Kits',
      '10',
      'Platforms',
      'UAP',
    );

    if (!shell.test('-e', uapFolderPath)) {
      // Check other installation folder from reg
      const sdkFolder = getSDK10InstallationFolder();
      if (sdkFolder) {
        uapFolderPath = path.join(sdkFolder, 'Platforms', 'UAP');
      }
    }

    // No UAP SDK exists on this machine
    if (!shell.test('-e', uapFolderPath)) {
      return results;
    }

    shell
      .ls(uapFolderPath)
      .filter(uapDir => shell.test('-d', path.join(uapFolderPath, uapDir)))
      .map(Version.tryParse)
      .forEach(version => version && results.push(version));

    return results;
  }
}

function getVCToolsByArch(buildArch: BuildArch): string {
  switch (buildArch) {
    case 'x86':
    case 'x64':
      return 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64';
    case 'ARM':
      return 'Microsoft.VisualStudio.Component.VC.Tools.ARM';
    case 'ARM64':
      return 'Microsoft.VisualStudio.Component.VC.Tools.ARM64';
  }
}

function getSDK10InstallationFolder(): string {
  const folder = '';

  const execString =
    'reg query "HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v10.0" /s /v InstallationFolder /reg:32';
  let output;
  try {
    output = execSync(execString).toString();
  } catch (e) {
    return folder;
  }

  const re = /\\Microsoft SDKs\\Windows\\v10.0\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
  const match = re.exec(output);
  if (match) {
    return match[1];
  }

  return folder;
}
