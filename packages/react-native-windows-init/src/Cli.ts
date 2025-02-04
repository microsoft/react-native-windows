/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import yargs from 'yargs';
import fs from '@react-native-windows/fs';
import semver from 'semver';
import {execSync} from 'child_process';
import validUrl from 'valid-url';
import prompts from 'prompts';
import findUp from 'find-up';
import chalk from 'chalk';
import npmFetch from 'npm-registry-fetch';

import {
  Telemetry,
  CommandStartInfo,
  CommandEndInfo,
  CodedErrors,
  CodedError,
  yargsOptionsToOptions,
  optionsToArgs,
  configToProjectInfo,
  getProjectFileFromConfig,
  OptionSanitizer,
  YargsOptionsType,
  deviceArchitecture,
  nodeArchitecture,
} from '@react-native-windows/telemetry';

/**
 * Important:
 * Do not use process.exit() in this script as it will prevent telemetry from being sent.
 * See https://github.com/microsoft/ApplicationInsights-node.js/issues/580
 */

import requireGenerateWindows from './requireGenerateWindows';

let NPM_REGISTRY_URL = 'https://registry.npmjs.org';
try {
  const npmConfReg = execSync('npm config get registry').toString().trim();
  if (validUrl.isUri(npmConfReg)) {
    NPM_REGISTRY_URL = npmConfReg;
  }
} catch (error: any) {
  // Ignore workspace errors as `npm config` does not support it
  const stderr = error?.stderr?.toString() || '';
  if (!stderr.includes('ENOWORKSPACES')) {
    throw error;
  }
}

// Causes the type-checker to ensure the options object is a valid yargs options object
function initOptions<T extends Record<string, yargs.Options>>(options: T): T {
  return options;
}

export const windowsInitOptions = initOptions({
  version: {
    type: 'string',
    describe: 'The version of react-native-windows to use.',
  },
  namespace: {
    type: 'string',
    describe:
      "The native project namespace. This should be expressed using dots as separators. i.e. 'Level1.Level2.Level3'. The generator will apply the correct syntax for the target language",
  },
  verbose: {
    type: 'boolean',
    describe: 'Enables logging.',
    default: false,
  },
  telemetry: {
    type: 'boolean',
    describe:
      'Controls sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
    default: true,
  },
  language: {
    type: 'string',
    describe: 'The language the project is written in.',
    choices: ['cs', 'cpp'],
    default: 'cpp',
  },
  overwrite: {
    type: 'boolean',
    describe: 'Overwrite any existing files without prompting',
    default: false,
  },
  projectType: {
    type: 'string',
    describe: 'The type of project to initialize (supported on 0.64+)',
    choices: ['app', 'lib'],
    default: 'app',
  },
  experimentalNuGetDependency: {
    type: 'boolean',
    describe:
      '[Experimental] change to start consuming a NuGet containing a pre-built dll version of Microsoft.ReactNative',
    hidden: true,
    default: false,
  },
  useHermes: {
    type: 'boolean',
    describe:
      '[Experimental] Use Hermes instead of Chakra as the JS engine (supported on 0.64+ for C++ projects)',
    default: false,
  },
  useWinUI3: {
    type: 'boolean',
    describe: '[Experimental] Use WinUI 3 (Windows App SDK)',
    hidden: true,
    default: false,
  },
  nuGetTestVersion: {
    type: 'string',
    describe:
      '[internalTesting] By default the NuGet version matches the rnw package. This flag allows manually specifying the version for internal testing.',
    hidden: true,
  },
  nuGetTestFeed: {
    type: 'string',
    describe:
      '[internalTesting] Allows a test feed to be added to the generated NuGet configuration',
    hidden: true,
  },
  useDevMode: {
    type: 'boolean',
    describe:
      '[internalTesting] Link rather than Add/Install the react-native-windows package. This option is for the development workflow of the developers working on react-native-windows.',
    hidden: true,
    default: undefined, // This must be undefined because we define the conflicts field below. Defining a default here will break the version option
    conflicts: 'version',
  },
});

const yargsParser = yargs
  .version(false)
  .options(windowsInitOptions)
  .strict(true);

function getReactNativeProjectName(): string {
  console.log('Reading project name from package.json...');
  const cwd = process.cwd();
  const pkgJsonPath = findUp.sync('package.json', {cwd});
  if (!pkgJsonPath) {
    throw new CodedError(
      'NoPackageJson',
      'Unable to find package.json.  This should be run from within an existing react-native project.',
    );
  }
  type PackageJson = {name: string};

  let name = fs.readJsonFileSync<PackageJson>(pkgJsonPath).name;
  if (!name) {
    const appJsonPath = findUp.sync('app.json', {cwd});
    if (appJsonPath) {
      console.log('Reading project name from app.json...');
      name = fs.readJsonFileSync<PackageJson>(pkgJsonPath).name;
    }
  }
  if (!name) {
    console.error('Please specify name in package.json or app.json');
  }
  return name;
}

function getReactNativeVersion(): string {
  console.log('Reading react-native version from node_modules...');
  const rnPkgJsonPath = require.resolve('react-native/package.json', {
    paths: [process.cwd()],
  });
  if (fs.existsSync(rnPkgJsonPath)) {
    return require(rnPkgJsonPath).version;
  }

  throw new CodedError(
    'NoReactNativeFound',
    'Must be run from a project that already depends on react-native, and has react-native installed.',
  );
}

function getDefaultReactNativeWindowsSemVerForReactNativeVersion(
  rnVersion: string,
): string {
  const validVersion = semver.valid(rnVersion);
  if (validVersion) {
    const major = semver.major(validVersion);
    const minor = semver.minor(validVersion);
    if (major === 0 && minor >= 59) {
      return `^${major}.${minor}.0-0`;
    }
  }

  throw new CodedError(
    'UnsupportedReactNativeVersion',
    `Error: Unsupported version of react-native: ${chalk.cyan(
      rnVersion,
    )} react-native-windows supports react-native versions ${chalk.cyan(
      '>=0.60',
    )}`,
  );
}

function getMatchingReactNativeSemVerForReactNativeWindowsVersion(
  rnwVersion: string,
): string {
  const validVersion = semver.valid(rnwVersion);
  if (validVersion) {
    const major = semver.major(validVersion);
    const minor = semver.minor(validVersion);
    if (major === 0 && minor >= 59) {
      return `^${major}.${minor}`;
    }
  }
  return 'unknown';
}

async function getLatestMatchingVersion(
  pkg: string,
  versionSemVer: string,
): Promise<string> {
  const npmResponse = await npmFetch.json(pkg, {registry: NPM_REGISTRY_URL});

  // Check if versionSemVer is a tag (i.e. 'canary', 'latest', 'preview', etc.)
  if ('dist-tags' in npmResponse) {
    const distTags = npmResponse['dist-tags'] as Record<string, string>;
    if (versionSemVer in distTags) {
      return distTags[versionSemVer];
    }
  }

  // Check if versionSemVer is a semver version (i.e. '^0.60.0-0', '0.63.1', etc.)
  if ('versions' in npmResponse) {
    const versions = Object.keys(
      npmResponse.versions as Record<string, unknown>,
    );
    if (versions.length > 0) {
      const candidates = versions
        .filter(v => semver.satisfies(v, versionSemVer))
        .sort(semver.rcompare);
      if (candidates.length > 0) {
        return candidates[0];
      }
    }
  }

  throw new CodedError(
    'NoMatchingPackageVersion',
    `No matching version of ${pkg}@${versionSemVer} found`,
  );
}

async function getLatestRNWVersion(): Promise<string> {
  const rnwLatestVersion = await getLatestMatchingRNWVersion('latest');
  if (!rnwLatestVersion) {
    throw new CodedError(
      'NoLatestReactNativeWindows',
      'Error: No version of react-native-windows@latest found',
    );
  }
  return rnwLatestVersion;
}

async function getLatestMatchingRNWVersion(
  versionSemVer: string,
): Promise<string | null> {
  try {
    const version = await getLatestMatchingVersion(
      'react-native-windows',
      versionSemVer,
    );
    return version;
  } catch (err) {
    return null;
  }
}

function installReactNativeWindows(
  version: string | undefined,
  verbose: boolean,
  useDevMode: boolean,
) {
  const cwd = process.cwd();
  const execOptions = verbose ? {stdio: 'inherit' as 'inherit'} : {};

  if (useDevMode) {
    const packageCmd = isProjectUsingYarn(cwd) ? 'yarn' : 'npm';
    execSync(`${packageCmd} link react-native-windows`, execOptions);
    version = '*';
  } else if (!version) {
    throw new CodedError(
      'Unknown',
      'Unexpected error encountered. If you are able, please file an issue on: https://github.com/microsoft/react-native-windows/issues/new/choose',
    );
  }

  const parsedVersion = semver.parse(version);
  if (parsedVersion) {
    const newSteps =
      'Please see https://microsoft.github.io/react-native-windows/docs/getting-started for the latest method for adding RNW to your project.';
    if (
      parsedVersion.minor > 75 ||
      (parsedVersion.minor === 0 &&
        parsedVersion.prerelease.length > 1 &&
        parsedVersion.prerelease[0] === 'canary' &&
        typeof parsedVersion.prerelease[1] === 'number' &&
        parsedVersion.prerelease[1] > 843)
    ) {
      // Full-stop, you can't use the command anymore.
      throw new CodedError(
        'UnsupportedReactNativeVersion',
        `react-native-windows-init only supports react-native-windows <= 0.75. ${newSteps}`,
      );
    } else if (parsedVersion.minor === 75) {
      // You can use the command for now, but it will be deprecated soon.
      console.warn(
        chalk.yellow(
          `Warning: react-native-windows-init will be deprecated for RNW > 0.75. ${newSteps}`,
        ),
      );
    }
  }

  console.log(
    `Installing ${chalk.green('react-native-windows')}@${chalk.cyan(
      version,
    )}...`,
  );

  const pkgJsonPath = findUp.sync('package.json', {cwd});
  if (!pkgJsonPath) {
    throw new CodedError('NoPackageJson', 'Unable to find package.json');
  }

  const pkgJson = require(pkgJsonPath);

  // check how react-native is installed
  if ('dependencies' in pkgJson && 'react-native' in pkgJson.dependencies) {
    // regular dependency (probably an app), inject into json and run install
    pkgJson.dependencies['react-native-windows'] = version;
    fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));
    execSync(isProjectUsingYarn(cwd) ? 'yarn' : 'npm install', execOptions);
  } else if (
    'devDependencies' in pkgJson &&
    'react-native' in pkgJson.devDependencies
  ) {
    // only a dev dependency (probably a native module),
    execSync(
      isProjectUsingYarn(cwd)
        ? `yarn add react-native-windows@${version} --dev`
        : `npm install react-native-windows@${version} --save-dev`,
      execOptions,
    );
  } else {
    throw new CodedError(
      'NoReactNativeDependencies',
      "Unable to find 'react-native' in package.json's dependencies or devDependencies. This should be run from within an existing react-native app or lib.",
    );
  }

  console.log(
    chalk.green(
      `react-native-windows@${chalk.cyan(
        require(require.resolve('react-native-windows/package.json', {
          paths: [cwd],
        })).version,
      )} successfully installed.`,
    ),
  );
}

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(key: keyof WindowsInitOptions, value: any): any {
  // Do not add a default case here.
  // Strings risking PII should just return true if present, false otherwise.
  // All others should return the value (or false if undefined).
  switch (key) {
    case 'namespace':
    case 'nuGetTestFeed':
    case 'nuGetTestVersion':
      return value ? true : false;
    case 'verbose':
    case 'version':
    case 'telemetry':
    case 'language':
    case 'overwrite':
    case 'projectType':
    case 'experimentalNuGetDependency':
    case 'useHermes':
    case 'useWinUI3':
    case 'useDevMode':
      return value === undefined ? false : value;
  }
}

/**
 * Sets up and starts the telemetry gathering for the CLI command.
 * @param args The raw CLI args.
 * @param options The CLI args parsed by yargs.
 */
async function startTelemetrySession(
  args: string[],
  options: YargsOptionsType,
) {
  const verbose = options.verbose === true;

  if (!options.telemetry) {
    if (verbose) {
      console.log('Telemetry is disabled');
    }
    return;
  }

  if (verbose) {
    console.log(
      `Running ${nodeArchitecture()} node on a ${deviceArchitecture()} machine`,
    );
  }

  if (deviceArchitecture() !== nodeArchitecture()) {
    console.warn(
      'This version of node was built for a different architecture than this machine and may cause unintended behavior',
    );
  }

  // Setup telemetry, but don't get NPM package version info right away as
  // we're going to change things and this may interfere with the resolver
  await Telemetry.setup({populateNpmPackageVersions: false});

  const sanitizedOptions = yargsOptionsToOptions(
    options,
    optionSanitizer as OptionSanitizer,
  );
  const sanitizedDefaultOptions = yargsOptionsToOptions(
    yargsParser.parse(''),
    optionSanitizer as OptionSanitizer,
  );
  const sanitizedArgs = optionsToArgs(sanitizedOptions, args);

  const startInfo: CommandStartInfo = {
    commandName: 'react-native-windows-init',
    args: sanitizedArgs,
    options: sanitizedOptions,
    defaultOptions: sanitizedDefaultOptions,
  };

  Telemetry.startCommand(startInfo);
}

/**
 * Adds the new project's telemetry info by calling and processing `npx @react-native-community/cli config`.
 */
async function addProjectInfoToTelemetry() {
  if (!Telemetry.isEnabled()) {
    return;
  }

  try {
    const config = JSON.parse(
      execSync('npx @react-native-community/cli config', {
        stdio: ['ignore', 'pipe', 'ignore'],
      }).toString(),
    );
    const projectInfo = await configToProjectInfo(config);
    if (projectInfo) {
      Telemetry.setProjectInfo(projectInfo);
    }

    const projectFile = getProjectFileFromConfig(config);
    if (projectFile) {
      await Telemetry.populateNuGetPackageVersions(projectFile);
    }
  } catch {}
}

/**
 * Ends the gathering of telemetry for the CLI command.
 * @param error The error (if any) thrown during the command.
 */
function endTelemetrySession(error?: Error) {
  const endInfo: CommandEndInfo = {
    resultCode: 'Success',
  };

  if (error) {
    endInfo.resultCode =
      error instanceof CodedError ? (error as CodedError).type : 'Unknown';
  }

  Telemetry.endCommand(endInfo);
}

/**
 * Sets the process exit code and offers some information at the end of a CLI command.
 * @param loggingIsEnabled Is verbose logging enabled.
 * @param error The error caught during the process, if any.
 */
function setExitProcessWithError(
  loggingIsEnabled: boolean,
  error?: Error,
): void {
  if (error) {
    const errorType =
      error instanceof CodedError ? (error as CodedError).type : 'Unknown';

    process.exitCode = CodedErrors[errorType];

    if (loggingIsEnabled) {
      console.log(
        `Command failed with error ${chalk.bold(errorType)}: ${error.message}`,
      );
      if (Telemetry.isEnabled()) {
        console.log(
          `Your telemetry sessionId was ${chalk.bold(
            Telemetry.getSessionId(),
          )}`,
        );
      }
    } else {
      console.log(
        `Command failed. Re-run the command with ${chalk.bold(
          '--verbose',
        )} for more information.`,
      );
    }
  }
}

/**
 * Check if project is using Yarn (has `yarn.lock` in the tree)
 */
function isProjectUsingYarn(cwd: string): boolean {
  return !!findUp.sync('yarn.lock', {cwd});
}

export async function reactNativeWindowsInit(args?: string[]) {
  args = args ?? process.argv;
  const options = yargsParser.parse(args) as WindowsInitOptions;

  if (options.verbose) {
    console.log(options);
  }

  await startTelemetrySession(args, options);

  let initWindowsError: Error | undefined;
  try {
    const name = getReactNativeProjectName();
    const ns = options.namespace || name;
    const useDevMode = !!(options.useDevMode as unknown); // TS assumes the type is undefined
    let version = options.version;

    if (options.useWinUI3 && options.experimentalNuGetDependency) {
      throw new CodedError(
        'IncompatibleOptions',
        "Error: Incompatible options specified. Options '--useWinUI3' and '--experimentalNuGetDependency' are incompatible",
        {detail: 'useWinUI3 and experimentalNuGetDependency'},
      );
    }

    if (!useDevMode) {
      if (!version) {
        const rnVersion = getReactNativeVersion();
        version =
          getDefaultReactNativeWindowsSemVerForReactNativeVersion(rnVersion);
      }

      const rnwResolvedVersion = await getLatestMatchingRNWVersion(version);

      if (!rnwResolvedVersion) {
        if (options.version) {
          console.warn(
            `Warning: Querying npm to find react-native-windows@${options.version} failed.  Attempting to continue anyway...`,
          );
        } else {
          const rnwLatestVersion = await getLatestRNWVersion();
          throw new CodedError(
            'NoAutoMatchingReactNativeWindows',
            `
  No compatible version of ${chalk.green('react-native-windows')} found.
  The latest supported version is ${chalk.green(
    'react-native-windows',
  )}@${chalk.cyan(rnwLatestVersion)}.
  Please modify your application to use ${chalk.green(
    'react-native',
  )}@${chalk.cyan(
              getMatchingReactNativeSemVerForReactNativeWindowsVersion(
                rnwLatestVersion,
              ),
            )} or another supported version of ${chalk.green(
              'react-native',
            )} and try again.
  `,
            {rnwLatestVersion: rnwLatestVersion},
          );
        }
      }

      if (!options.version) {
        console.log(
          `Latest matching version of ${chalk.bold(
            'react-native-windows',
          )} for ${chalk.green('react-native')}@${chalk.cyan(
            getReactNativeVersion(),
          )} is ${chalk.green('react-native-windows')}@${chalk.cyan(
            rnwResolvedVersion,
          )}`,
        );

        if (rnwResolvedVersion && semver.prerelease(rnwResolvedVersion)) {
          const rnwLatestVersion = await getLatestRNWVersion();
          console.warn(
            `
  ${chalk.green('react-native-windows')}@${chalk.cyan(
              rnwResolvedVersion,
            )} is a ${chalk.yellow('pre-release')} version.
  The latest supported version is ${chalk.green(
    'react-native-windows',
  )}@${chalk.cyan(rnwLatestVersion)}.
  You can either downgrade your version of ${chalk.green(
    'react-native',
  )} to ${chalk.cyan(
              getMatchingReactNativeSemVerForReactNativeWindowsVersion(
                rnwLatestVersion,
              ),
            )}, or continue with a ${chalk.yellow(
              'pre-release',
            )} version of ${chalk.bold('react-native-windows')}.
  `,
          );

          const confirm: boolean = (
            await prompts({
              type: 'confirm',
              name: 'confirm',
              message: `Do you wish to continue with ${chalk.green(
                'react-native-windows',
              )}@${chalk.cyan(rnwResolvedVersion)}?`,
            })
          ).confirm;

          if (!confirm) {
            throw new CodedError('UserCancel', 'User canceled');
          }
        }
      }

      if (rnwResolvedVersion) {
        version = rnwResolvedVersion;
      }
    }

    installReactNativeWindows(version, options.verbose, useDevMode);

    const generateWindows = requireGenerateWindows();

    // Now that new NPM packages have been installed, get their versions
    await Telemetry.populateNpmPackageVersions();

    await generateWindows(process.cwd(), name, ns, {
      language: options.language as 'cs' | 'cpp',
      overwrite: options.overwrite,
      verbose: options.verbose,
      projectType: options.projectType as 'lib' | 'app',
      experimentalNuGetDependency: options.experimentalNuGetDependency,
      useWinUI3: options.useWinUI3,
      useHermes: options.useHermes,
      useDevMode: useDevMode,
      nuGetTestVersion: options.nuGetTestVersion,
      nuGetTestFeed: options.nuGetTestFeed,
      telemetry: options.telemetry,
    });

    // Now that the project has been generated, add project info
    await addProjectInfoToTelemetry();
  } catch (ex) {
    // Since we may have failed before generating a project, make
    // sure we get those NPM package versions
    await Telemetry.populateNpmPackageVersions();

    initWindowsError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(initWindowsError);

    console.error(chalk.red(initWindowsError.message));
    console.error(initWindowsError);
  }
  endTelemetrySession(initWindowsError);
  setExitProcessWithError(options.verbose, initWindowsError);
}

export type WindowsInitOptions = yargs.InferredOptionTypes<
  typeof windowsInitOptions
>;
