/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as fs from 'fs';
import {
  telemetryClient,
  isMSFTInternal,
  getDiskFreeSpace,
} from '@react-native-windows/telemetry';

import * as build from './utils/build';
import * as chalk from 'chalk';
import * as deploy from './utils/deploy';
import {newError, newInfo} from './utils/commandWithProgress';
import * as info from './utils/info';
import MSBuildTools from './utils/msbuildtools';

import {Command, Config} from '@react-native-community/cli-types';
import {runWindowsOptions, RunWindowsOptions} from './runWindowsOptions';

import {autoLinkCommand} from './utils/autolink';
import {totalmem, cpus} from 'os';

function setExitProcessWithError(loggingWasEnabled: boolean): void {
  if (!loggingWasEnabled) {
    console.log(
      `Re-run the command with ${chalk.bold('--logging')} for more information`,
    );
  }
  process.exitCode = 1;
}

function getPkgVersion(pkgName: string): string {
  try {
    const pkgJsonPath = require.resolve(`${pkgName}/package.json`, {
      paths: [process.cwd(), __dirname],
    });
    const pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath).toString());
    if (pkgJson.name === pkgName && pkgJson.version !== undefined) {
      return pkgJson.version;
    }
  } catch {}
  newWarn(`Could not determine ${pkgName} version`);
  return '';
}

enum RunWindowsPhase {
  None,
  AutoLink,
  FindBuildTools,
  NuGetRestore,
  FindSolution,
  Deploy,
}

let runWindowsPhase = RunWindowsPhase.None;
/**
 * Performs build deploy and launch of RNW apps.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function runWindows(
  args: string[],
  config: Config,
  options: RunWindowsOptions,
) {
  if (!options.telemetry || process.env.AGENT_NAME) {
    if (options.logging) {
      console.log('Disabling telemetry');
    }
    telemetryClient.config.disableAppInsights = true;
  }

  if (options.info) {
    try {
      const output = await info.getEnvironmentInfo();
      console.log(output.trimEnd());
      console.log('  Installed UWP SDKs:');
      const sdks = MSBuildTools.getAllAvailableUAPVersions();
      sdks.forEach(version => console.log('    ' + version));
      return;
    } catch (e) {
      telemetryClient.trackException({exception: e});
      newError('Unable to print environment info.\n' + e.toString());
      return setExitProcessWithError(options.logging);
    }
  }

  let runWindowsError;
  try {
    await runWindowsInternal(args, config, options);
  } catch (e) {
    telemetryClient.trackException({exception: e});
    runWindowsError = e;
    return setExitProcessWithError(options.logging);
  } finally {
    telemetryClient.trackEvent({
      name: 'run-windows',
      properties: {
        release: options.release,
        root: options.root !== undefined,
        arch: options.arch,
        singleproc: options.singleproc,
        emulator: options.emulator,
        device: options.device,
        target: options.target,
        remoteDebugging: options.remoteDebugging,
        logging: options.logging,
        packager: options.packager,
        bundle: options.bundle,
        launch: options.launch,
        autolink: options.autolink,
        build: options.bundle,
        deploy: options.deploy,
        sln: options.sln !== undefined,
        proj: options.proj !== undefined,
        msBuildProps:
          options.msbuildprops !== undefined
            ? options.msbuildprops!.split(',').length
            : 0,
        info: options.info,
        directDebugging: options.directDebugging,
        'react-native-windows': getPkgVersion('react-native-windows'),
        'react-native': getPkgVersion('react-native'),
        'cli-version': getPkgVersion('@react-native-windows/cli'),
        msftInternal: isMSFTInternal(),
        durationInSecs: process.uptime(),
        success: runWindowsError === undefined,
        phase: RunWindowsPhase[runWindowsPhase],
        totalMem: totalmem(),
        diskFree: getDiskFreeSpace(__dirname),
        cpus: cpus().length,
      },
    });
    telemetryClient.flush();
  }
}

async function runWindowsInternal(
  args: string[],
  config: Config,
  options: RunWindowsOptions,
) {
  const verbose = options.logging;

  if (verbose) {
    newInfo('Verbose: ON');
  }

  // Get the solution file
  const slnFile = build.getAppSolutionFile(options, config);

  if (options.autolink) {
    const autolinkArgs: string[] = [];
    const autolinkConfig = config;
    const autoLinkOptions = {
      logging: options.logging,
      proj: options.proj,
      sln: options.sln,
    };
    runWindowsPhase = RunWindowsPhase.AutoLink;
    await autoLinkCommand.func(autolinkArgs, autolinkConfig, autoLinkOptions);
  } else {
    newInfo('Autolink step is skipped');
  }

  let buildTools: MSBuildTools;
  runWindowsPhase = RunWindowsPhase.FindBuildTools;
  buildTools = MSBuildTools.findAvailableVersion(options.arch, verbose);

  if (options.build) {
    runWindowsPhase = RunWindowsPhase.FindSolution;
    if (!slnFile) {
      newError(
        'Visual Studio Solution file not found. Maybe run "npx react-native-windows-init" first?',
      );
      throw new Error('Cannot find solution file');
    }

    try {
      runWindowsPhase = RunWindowsPhase.NuGetRestore;
      await build.restoreNuGetPackages(slnFile!, buildTools, verbose);
    } catch (e) {
      newError('Failed to restore the NuGet packages: ' + e.toString());
      throw e;
    }

    // Get build/deploy options
    const buildType = deploy.getBuildConfiguration(options);
    let msBuildProps = build.parseMsBuildProps(options);

    // Disable the autolink check since we just ran it
    msBuildProps.RunAutolinkCheck = 'false';

    try {
      runWindowsPhase = RunWindowsPhase.FindSolution;
      await build.buildSolution(
        buildTools,
        slnFile!,
        buildType,
        options.arch,
        msBuildProps,
        verbose,
        undefined, // build the default target
        options.buildLogDirectory,
      );
    } catch (e) {
      newError(
        `Build failed with message ${
          e.message
        }. Check your build configuration.`,
      );
      if (e.logfile) {
        console.log('See', chalk.bold(e.logfile));
      }
      throw e;
    }
  } else {
    newInfo('Build step is skipped');
  }

  await deploy.startServerInNewWindow(options, verbose);

  if (options.deploy) {
    runWindowsPhase = RunWindowsPhase.FindSolution;
    if (!slnFile) {
      newError(
        'Visual Studio Solution file not found. Maybe run "npx react-native-windows-init" first?',
      );
      throw new Error('Cannot find solution file');
    }

    try {
      runWindowsPhase = RunWindowsPhase.Deploy;
      if (options.device || options.emulator || options.target) {
        await deploy.deployToDevice(options, verbose);
      } else {
        await deploy.deployToDesktop(options, verbose, config, buildTools);
      }
    } catch (e) {
      newError(`Failed to deploy${e ? `: ${e.message}` : ''}`);
      throw e;
    }
  } else {
    newInfo('Deploy step is skipped');
  }
}

/*
// Example of running the Windows Command
runWindows({
  root: 'C:\\github\\hack\\myapp',
  debug: true,
  arch: 'x86',
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe'
});
*/

/**
 * Starts the app on a connected Windows emulator or mobile device.
 */
export const runWindowsCommand: Command = {
  name: 'run-windows',
  description:
    'builds your app and starts it on a connected Windows desktop, emulator or device',
  func: runWindows,
  options: runWindowsOptions,
};
