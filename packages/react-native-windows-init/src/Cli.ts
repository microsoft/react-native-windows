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
} from '@react-native-windows/telemetry';

/**
 * Important:
 * Do not use process.exit() in this script as it will prevent telemetry from being sent.
 * See https://github.com/microsoft/ApplicationInsights-node.js/issues/580
 */

import requireGenerateWindows from './requireGenerateWindows';

const npmConfReg = execSync('npm config get registry')
  .toString()
  .trim();
const NPM_REGISTRY_URL = validUrl.isUri(npmConfReg)
  ? npmConfReg
  : 'http://registry.npmjs.org';

const yargsParser = yargs
  .version(false)
  .strict(true)
  .options({
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
      describe: '[Experimental] Use WinUI3',
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
      conflicts: 'version',
    },
  })
  .strict(true);

const argv = yargsParser.argv;

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
  useDevMode: boolean,
) {
  const cwd = process.cwd();
  const execOptions = argv.verbose ? {stdio: 'inherit' as 'inherit'} : {};

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
 * Sanitizes the given option for telemetery.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(key: string, value: any): any {
  switch (key) {
    case 'namespace':
      return value === undefined ? false : true;
    default:
      return value === undefined ? false : value;
  }
}

/**
 * Sets up and starts the telemetry gathering for the CLI command.
 */
async function startTelemetrySession() {
  if (!argv.telemetry) {
    if (argv.verbose) {
      console.log('Telemetry is disabled');
    }
    return;
  }

  await Telemetry.setup();

  const sanitizedOptions = yargsOptionsToOptions(argv, optionSanitizer);
  const sanitizedDefaultOptions = yargsOptionsToOptions(
    yargsParser.parse(''),
    optionSanitizer,
  );
  const sanitizedArgs = optionsToArgs(sanitizedOptions, process.argv);

  const startInfo: CommandStartInfo = {
    commandName: 'react-native-windows-init',
    args: sanitizedArgs,
    options: sanitizedOptions,
    defaultOptions: sanitizedDefaultOptions,
  };

  Telemetry.startCommand(startInfo);
}

/**
 * Adds the new project's telemetry info by calling and processing `react-native config`.
 */
async function addProjectInfoToTelemetry() {
  if (!Telemetry.isEnabled()) {
    return;
  }

  try {
    const config = JSON.parse(
      execSync('npx react-native config', {
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

(async () => {
  if (argv.verbose) {
    console.log(argv);
  }

  await startTelemetrySession();

  let initWindowsError: Error | undefined;
  try {
    const name = getReactNativeProjectName();
    const ns = argv.namespace || name;
    const useDevMode = !!argv.useDevMode;
    let version = argv.version;

    if (argv.useWinUI3 && argv.experimentalNuGetDependency) {
      throw new CodedError(
        'IncompatibleOptions',
        "Error: Incompatible options specified. Options '--useWinUI3' and '--experimentalNuGetDependency' are incompatible",
        {detail: 'useWinUI3 and experimentalNuGetDependency'},
      );
    }

    if (!useDevMode) {
      if (!version) {
        const rnVersion = getReactNativeVersion();
        version = getDefaultReactNativeWindowsSemVerForReactNativeVersion(
          rnVersion,
        );
      }

      const rnwResolvedVersion = await getLatestMatchingRNWVersion(version);

      if (!rnwResolvedVersion) {
        if (argv.version) {
          console.warn(
            `Warning: Querying npm to find react-native-windows@${argv.version} failed.  Attempting to continue anyway...`,
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

      if (!argv.version) {
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

    installReactNativeWindows(version, useDevMode);

    const generateWindows = requireGenerateWindows();

    // Now that new NPM packages have been installed, refresh their versions
    await Telemetry.populateNpmPackageVersions(true);

    await generateWindows(process.cwd(), name, ns, {
      language: argv.language as 'cs' | 'cpp',
      overwrite: argv.overwrite,
      verbose: argv.verbose,
      projectType: argv.projectType as 'lib' | 'app',
      experimentalNuGetDependency: argv.experimentalNuGetDependency,
      useWinUI3: argv.useWinUI3,
      useHermes: argv.useHermes,
      useDevMode: useDevMode,
      nuGetTestVersion: argv.nuGetTestVersion,
      nuGetTestFeed: argv.nuGetTestFeed,
      telemetry: argv.telemetry,
    });

    // Now that the project has been generated, add project info
    await addProjectInfoToTelemetry();
  } catch (ex) {
    initWindowsError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(initWindowsError);

    console.error(chalk.red(initWindowsError.message));
    console.error(initWindowsError);
  }
  endTelemetrySession(initWindowsError);
  setExitProcessWithError(argv.verbose, initWindowsError);
})();
