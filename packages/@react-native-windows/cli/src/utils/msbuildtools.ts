/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {totalmem, EOL} from 'os';
import fs from '@react-native-windows/fs';
import path from 'path';
import child_process from 'child_process';
import chalk from 'chalk';
import os from 'os';
import shell from 'shelljs';
import Version from './version';
import * as checkRequirements from './checkRequirements';
import {
  commandWithProgress,
  newInfo,
  newSpinner,
  newSuccess,
  newError,
  powershell,
} from './commandWithProgress';
import {execSync} from 'child_process';
import {BuildArch, BuildConfig} from '../commands/runWindows/runWindowsOptions';
import {findLatestVsInstall} from './vsInstalls';
import {CodedError} from '@react-native-windows/telemetry';

export default class MSBuildTools {
  /**
   * @param version is something like 17.0 for 2022
   * @param installationPath  Path to installation root
   * @param installationVersion is the full version e.g. 17.3.32929.385
   */
  constructor(
    public readonly version: string,
    public readonly installationPath: string,
    public readonly installationVersion: string,
  ) {}

  /**
   * @returns directory where x64 msbuild can be found
   */
  msbuildPath() {
    return path.join(this.installationPath, 'MSBuild/Current/Bin/amd64');
  }

  cleanProject(slnFile: string) {
    const cmd = `"${path.join(
      this.msbuildPath(),
      'msbuild.exe',
    )}" "${slnFile}" /t:Clean`;
    const results = child_process.execSync(cmd).toString().split(EOL);
    results.forEach(result => console.log(chalk.white(result)));
  }

  async restorePackageConfigs(
    slnFile: any,
    buildArch: BuildArch,
    buildType: BuildConfig,
  ) {
    const text = 'Restoring NuGet packages ';
    const spinner = newSpinner(text);
    await commandWithProgress(
      spinner,
      text,
      path.join(this.msbuildPath(), 'msbuild.exe'),
      [
        slnFile,
        '/t:Restore',
        '/p:RestoreProjectStyle=PackagesConfig',
        '/p:RestorePackagesConfig=true',
        `/p:Platform=${buildArch}`,
        `/p:Configuration=${buildType}`,
      ],
      true,
      'MSBuildError',
    );
  }

  async buildProject(
    slnFile: string,
    buildType: BuildConfig,
    buildArch: BuildArch,
    msBuildProps: Record<string, string>,
    verbose: boolean,
    target: 'build' | 'deploy',
    buildLogDirectory: string | undefined,
    singleproc?: boolean,
  ) {
    newSuccess(`Found Solution: ${slnFile}`);
    newInfo(`Build configuration: ${buildType}`);
    newInfo(`Build platform: ${buildArch}`);

    const verbosityOption = verbose ? 'normal' : 'minimal';
    const logPrefix = path.join(
      buildLogDirectory || os.tmpdir(),
      `msbuild_${process.pid}_${target}`,
    );

    const errorLog = logPrefix + '.err';
    const warnLog = logPrefix + '.wrn';

    const localBinLog = target === 'build' ? '' : ':deploy.binlog';
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
    const enableParallelBuilds = singleproc !== true && highMemory;

    if (enableParallelBuilds) {
      args.push('/maxCpuCount');
    }

    if (target === 'build') {
      args.push('/restore', '/p:RestorePackagesConfig=true');
    } else {
      args.push(`/t:Deploy`);
    }

    Object.keys(msBuildProps).forEach(key => {
      args.push(`/p:${key}=${msBuildProps[key]}`);
    });

    try {
      checkRequirements.isWinSdkPresent('10.0');
    } catch (e) {
      newError((e as Error).message);
      throw e;
    }

    if (verbose) {
      console.log(`Running MSBuild with args ${args.join(' ')}`);
    }

    const progressName =
      target === 'deploy' ? 'Deploying Solution' : 'Building Solution';
    const spinner = newSpinner(progressName);
    try {
      await commandWithProgress(
        spinner,
        progressName,
        path.join(this.msbuildPath(), 'msbuild.exe'),
        [slnFile].concat(args),
        verbose,
        'MSBuildError',
      );
    } catch (e) {
      let error = e;
      if (e instanceof CodedError) {
        const origCodedError = e as CodedError;
        if (origCodedError.type === 'MSBuildError') {
          // Try to parse msbuild errors from errorLog
          const errorLogContents = (await fs.readFile(errorLog))
            .toString()
            .split(EOL)
            .filter(s => s)
            .map(s => s.trim());
          if (errorLogContents.length > 0) {
            const firstMessage = errorLogContents[0];
            error = new CodedError(
              'MSBuildError',
              firstMessage,
              origCodedError.data,
            );
            // Hide error messages in a field that won't automatically get reported
            // with telemetry but is still available to be parsed and sanitized
            (error as any).msBuildErrorMessages = errorLogContents;
          }
        }
      }
      throw error;
    }
    // If we have no errors, delete the error log when we're done
    if ((await fs.stat(errorLog)).size === 0) {
      await fs.unlink(errorLog);
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
    const minVersion =
      process.env.MinimumVisualStudioVersion ||
      process.env.VisualStudioVersion ||
      '17.11.0';
    const vsInstallation = findLatestVsInstall({
      requires,
      minVersion,
      verbose,
      prerelease,
    });

    if (!vsInstallation) {
      if (process.env.MinimumVisualStudioVersion != null) {
        throw new CodedError(
          'NoMSBuild',
          `MSBuild tools not found for version ${process.env.MinimumVisualStudioVersion} (from environment). Make sure all required components have been installed`,
          {
            MinimumVisualStudioVersionFromEnv:
              process.env.MinimumVisualStudioVersion,
          },
        );
      } else if (process.env.VisualStudioVersion != null) {
        throw new CodedError(
          'NoMSBuild',
          `MSBuild tools not found for version ${process.env.VisualStudioVersion} (from environment). Make sure all required components have been installed`,
          {VisualStudioVersionFromEnv: process.env.VisualStudioVersion},
        );
      } else {
        throw new CodedError(
          'NoMSBuild',
          `Could not find MSBuild with VCTools for Visual Studio ${minVersion} or later. Make sure all required components have been installed`,
          {minVersion: minVersion},
        );
      }
    }

    const toolsPath = path.join(
      vsInstallation.installationPath,
      'MSBuild/Current/Bin/amd64',
    );

    if (fs.existsSync(toolsPath)) {
      if (verbose) {
        newSuccess(
          `Found compatible MSBuild at ${toolsPath} (${vsInstallation.installationVersion})`,
        );
      }
      return new MSBuildTools(
        minVersion,
        vsInstallation.installationPath,
        vsInstallation.installationVersion,
      );
    } else {
      throw new CodedError(
        'NoMSBuild',
        `MSBuild path '${toolsPath} does not exist'`,
      );
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

  evaluateMSBuildProperties(
    solutionFile: string,
    projectFile: string,
    propertyNames?: string[],
    extraMsBuildProps?: Record<string, string>,
  ): Record<string, string> {
    const spinner = newSpinner('Running Eval-MsBuildProperties.ps1');

    try {
      const msbuildEvalScriptPath = path.resolve(
        __dirname,
        '..',
        '..',
        'src',
        'powershell',
        'Eval-MsBuildProperties.ps1',
      );

      let command = `${powershell} -ExecutionPolicy Unrestricted -NoProfile "${msbuildEvalScriptPath}" -SolutionFile '${solutionFile}' -ProjectFile '${projectFile}' -MSBuildPath '${this.msbuildPath()}'`;

      if (propertyNames && propertyNames.length > 0) {
        command += ` -PropertyNames '${propertyNames.join(',')}'`;
      }

      if (extraMsBuildProps) {
        command += " -ExtraMSBuildProps '";
        for (const extraProp in extraMsBuildProps) {
          if (!(extraProp in Object.prototype)) {
            command += `,${extraProp}=${extraMsBuildProps[extraProp]}`;
          }
        }
        command += "'";
      }

      const commandOutput = execSync(command).toString();
      spinner.succeed();

      const properties = JSON.parse(commandOutput) as Record<string, string>;
      spinner.succeed();
      return properties;
    } catch (e) {
      spinner.fail(
        'Running Eval-MsBuildProperties.ps1 failed: ' + (e as Error).message,
      );
      throw e;
    }
  }
}

function getVCToolsByArch(buildArch: BuildArch): string {
  switch (buildArch) {
    case 'x86':
    case 'x64':
      return 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64';
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

  const re =
    /\\Microsoft SDKs\\Windows\\v10.0\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
  const match = re.exec(output);
  if (match) {
    return match[1];
  }

  return folder;
}
