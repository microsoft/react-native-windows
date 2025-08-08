/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

/* eslint-disable complexity */

import fs from '@react-native-windows/fs';
import path from 'path';

import {Telemetry, CodedError} from '@react-native-windows/telemetry';
import type {Command, Config} from '@react-native-community/cli-types';

import * as build from '../../utils/build';
import chalk from 'chalk';
import * as deploy from '../../utils/deploy';
import {
  newError,
  newInfo,
  newWarn,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import * as pathHelpers from '../../utils/pathHelpers';
import * as info from '../../utils/info';
import {promptForArchitectureChoice} from '../../utils/architecturePrompt';
import MSBuildTools from '../../utils/msbuildtools';
import type {RunWindowsOptions} from './runWindowsOptions';
import {runWindowsOptions} from './runWindowsOptions';
import {autolinkWindowsInternal} from '../autolinkWindows/autolinkWindows';
import type {AutoLinkOptions} from '../autolinkWindows/autolinkWindowsOptions';

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(key: keyof RunWindowsOptions, value: any): any {
  // Do not add a default case here.
  // Strings risking PII should just return true if present, false otherwise.
  // All others should return the value (or false if undefined).
  switch (key) {
    case 'root':
    case 'target':
    case 'sln':
    case 'proj':
    case 'buildLogDirectory':
      return value === undefined ? false : true; // Strip PII
    case 'msbuildprops':
      return typeof value === 'string' ? value.split(',').length : 0; // Convert to count
    case 'release':
    case 'arch':
    case 'singleproc':
    case 'emulator':
    case 'device':
    case 'remoteDebugging':
    case 'logging':
    case 'packager':
    case 'bundle':
    case 'launch':
    case 'autolink':
    case 'build':
    case 'deploy':
    case 'deployFromLayout':
    case 'info':
    case 'directDebugging':
    case 'telemetry':
      return value === undefined ? false : value; // Return value
  }
}

/**
 * Get the extra props to add to the `run-windows` telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {
    phase: runWindowsPhase,
    hasRunRnwDependencies,
    msBuildProps: evaluateMSBuildPropsCallback
      ? evaluateMSBuildPropsCallback()
      : {},
  };
  return extraProps;
}

/**
 * Labels used by telemetry to represent current operation
 */
type RunWindowsPhase =
  | 'None'
  | 'Info'
  | 'FindSolution'
  | 'FindBuildTools'
  | 'Autolink'
  | 'RestorePackagesConfig'
  | 'Build'
  | 'Deploy';

let runWindowsPhase: RunWindowsPhase = 'None';

let hasRunRnwDependencies: boolean = false;

let evaluateMSBuildPropsCallback:
  | (() => Record<string, string> | null)
  | undefined;

/**
 * The function run when calling `npx @react-native-community/cli run-windows`.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function runWindows(
  args: string[],
  config: Config,
  options: RunWindowsOptions,
) {
  await startTelemetrySession(
    'run-windows',
    config,
    options,
    getDefaultOptions(config, runWindowsOptions),
    optionSanitizer,
  );

  // https://github.com/yarnpkg/yarn/issues/8334 - Yarn on Windows breaks apps that read from the environment variables
  // Yarn will run node via CreateProcess and pass npm_config_* variables in lowercase without unifying their value
  // with their possibly existing uppercase counterparts. This breaks programs that read from the environment block
  // and write to a case-insensitive dictionary since they expect to encounter each variable only once.
  // The values of the lowercase variables are the one npm actually wants to use, plus they are seeded from the
  // uppercase variable values one if there are no overrides.
  delete process.env.NPM_CONFIG_CACHE;
  delete process.env.NPM_CONFIG_PREFIX;

  if (process.env.LocalAppData) {
    hasRunRnwDependencies = fs.existsSync(
      path.join(process.env.LocalAppData, 'rnw-dependencies.txt'),
    ); // CODESYNC \vnext\scripts\rnw-dependencies.ps1
  }

  let runWindowsError: Error | undefined;
  if (options.info) {
    runWindowsPhase = 'Info';
    try {
      const output = await info.getEnvironmentInfo();
      console.log(output.trimEnd());
      console.log('  Installed UWP SDKs:');
      const sdks = MSBuildTools.getAllAvailableUAPVersions();
      sdks.forEach(version => console.log('    ' + version));
    } catch (ex) {
      runWindowsError =
        ex instanceof Error ? (ex as Error) : new Error(String(ex));
      Telemetry.trackException(runWindowsError);

      newError(
        'Unable to print environment info.\n' + runWindowsError.toString(),
      );
    }
    await endTelemetrySession(runWindowsError, getExtraProps);
    setExitProcessWithError(options.logging, runWindowsError);
    return;
  }

  try {
    await runWindowsInternal(args, config, options);
  } catch (ex) {
    runWindowsError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(runWindowsError);

    if (!hasRunRnwDependencies) {
      const rnwDependenciesPath = path.join(
        pathHelpers.resolveRnwRoot([process.cwd(), __dirname]),
        'scripts/rnw-dependencies.ps1',
      );

      newError(
        `It is possible your installation is missing required software dependencies. Dependencies can be automatically installed by running ${rnwDependenciesPath} from an elevated PowerShell prompt.\nFor more information, go to http://aka.ms/rnw-deps`,
      );
    }
  }
  await endTelemetrySession(runWindowsError, getExtraProps);
  setExitProcessWithError(options.logging, runWindowsError);
}

/**
 * Performs build deploy and launch of RNW apps.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function runWindowsInternal(
  args: string[],
  config: Config,
  options: RunWindowsOptions,
) {
  const verbose = options.logging === true;

  if (verbose) {
    newInfo('Verbose: ON');
  }

  // Warn about old architecture projects
  if (config.project.windows?.rnwConfig?.projectArch === 'old') {
    const promptResult = await promptForArchitectureChoice(
      'old architecture project',
    );

    if (
      !promptResult.shouldContinueWithOldArch &&
      !promptResult.userCancelled
    ) {
      // For existing projects, we can't change the architecture automatically
      // but we can provide guidance
      newInfo(
        'Note: For existing projects, you will need to manually migrate to the New Architecture. Please follow the migration guide at: https://microsoft.github.io/react-native-windows/docs/new-architecture',
      );
    }
  }

  // Get the solution file
  let slnFile: string | null;
  runWindowsPhase = 'FindSolution';
  try {
    slnFile = build.getAppSolutionFile(options, config);
  } catch (e) {
    newError(`Couldn't get app solution information. ${(e as Error).message}`);
    throw e;
  }

  let buildTools: MSBuildTools;
  runWindowsPhase = 'FindBuildTools';
  try {
    buildTools = MSBuildTools.findAvailableVersion(options.arch, verbose);
  } catch (error) {
    newWarn('No public VS release found');
    // Try prerelease
    try {
      newInfo('Trying pre-release VS');
      buildTools = MSBuildTools.findAvailableVersion(
        options.arch,
        verbose,
        true, // preRelease
      );
    } catch (e) {
      newError((e as Error).message);
      throw error;
    }
  }

  // Set up the callback to capture MSBuild properties after the command completes
  evaluateMSBuildPropsCallback = () => {
    const projectFile = build.getAppProjectFile(options, config);
    if (projectFile) {
      if (verbose) {
        newInfo('Gathering MSBuild data for telemetry.');
      }

      const msBuildPropertiesJsonPath = path.resolve(
        path.dirname(projectFile),
        'Generated Files',
        'msbuildproperties.g.json',
      );

      if (fs.existsSync(msBuildPropertiesJsonPath)) {
        if (verbose) {
          newInfo('Loading properties from msbuildproperties.g.json');
        }
        return fs.readJsonFileSync(msBuildPropertiesJsonPath);
      }

      if (verbose) {
        newInfo('Unable to find msbuildproperties.g.json');
      }
    }

    return {};
  };

  // Restore packages.config files for dependencies that don't support PackageReference.
  runWindowsPhase = 'RestorePackagesConfig';
  const buildType = deploy.getBuildConfiguration(options);
  try {
    await buildTools.restorePackageConfigs(slnFile, options.arch, buildType);
  } catch (e) {
    newError(
      `Couldn't restore found packages.config instances. ${
        (e as Error).message
      }`,
    );
    throw e;
  }

  if (options.autolink) {
    runWindowsPhase = 'Autolink';
    try {
      const autolinkArgs: string[] = [];
      const autolinkConfig = config;
      const autolinkOptions: AutoLinkOptions = {
        logging: options.logging,
        check: false,
        proj: options.proj,
        sln: options.sln,
        telemetry: options.telemetry,
      };
      await autolinkWindowsInternal(
        autolinkArgs,
        autolinkConfig,
        autolinkOptions,
      );
    } catch (e) {
      newError(`Autolinking failed. ${(e as Error).message}`);
      throw e;
    }
  } else {
    newInfo('Autolink step is skipped');
  }

  if (options.build) {
    runWindowsPhase = 'Build';
    if (!slnFile) {
      newError(
        'Visual Studio Solution file not found. Maybe run "npx @react-native-community/cli init-windows" first?',
      );
      throw new CodedError('NoSolution', 'Cannot find solution file');
    }

    // Get build/deploy options
    const msBuildProps = build.parseMsBuildProps(options);

    // Disable the autolink check since we just ran it
    msBuildProps.RunAutolinkCheck = 'false';

    try {
      await build.buildSolution(
        buildTools,
        slnFile!,
        buildType,
        options.arch,
        msBuildProps,
        verbose,
        'build',
        options.buildLogDirectory,
        options.singleproc,
      );
    } catch (e) {
      newError(
        `Build failed with message ${
          (e as Error).message
        }. Check your build configuration.`,
      );
      if ((e as any).logfile) {
        console.log('See', chalk.bold((e as any).logfile));
      }
      throw e;
    }
  } else {
    newInfo('Build step is skipped');
  }

  if (shouldLaunchPackager(options)) {
    await deploy.startServerInNewWindow(options, verbose);
  }

  if (options.deploy) {
    runWindowsPhase = 'Deploy';
    if (!slnFile) {
      newError(
        'Visual Studio Solution file not found. Maybe run "npx @react-native-community/cli init-windows" first?',
      );
      throw new CodedError('NoSolution', 'Cannot find solution file');
    }

    try {
      if (options.device || options.emulator || options.target) {
        await deploy.deployToDevice(options, verbose, config);
      } else {
        await deploy.deployToDesktop(options, verbose, config, buildTools);
      }
    } catch (e) {
      newError(`Failed to deploy${e ? `: ${(e as Error).message}` : ''}`);
      throw e;
    }
  } else {
    newInfo('Deploy step is skipped');
  }
}

function shouldLaunchPackager(options: RunWindowsOptions): boolean {
  return (
    options.packager === true &&
    options.launch === true &&
    options.release !== true
  );
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
    'Builds your RNW app and starts it on a connected Windows desktop, emulator or device',
  func: runWindows,
  options: runWindowsOptions,
};
