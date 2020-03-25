/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as yargs from 'yargs';
import * as fs from 'fs';
import * as semver from 'semver';
import {execSync} from 'child_process';
import * as validUrl from 'valid-url';
import * as prompts from 'prompts';
import * as findUp from 'find-up';
import * as chalk from 'chalk';

const Registry = require('npm-registry');

let npmConfReg = execSync('npm config get registry')
  .toString()
  .trim();
let NPM_REGISTRY_URL = validUrl.isUri(npmConfReg)
  ? npmConfReg
  : 'http://registry.npmjs.org';
const npm = new Registry({registry: NPM_REGISTRY_URL});

const argv = yargs.options({
  version: {
    type: 'string',
    describe: 'The version of react-native-windows to use.',
  },
  namespace: {type: 'string', describe: 'The native project namespace.'},
  verbose: {type: 'boolean', describe: 'Enables logging.'},
  language: {
    type: 'string',
    describe: 'Which language the app is written in.',
    choices: ['cs', 'cpp'],
    default: 'cpp',
  },
  overwrite: {
    type: 'boolean',
    describe: 'Overwrite any existing files without prompting',
  },
}).argv;

const EXITCODE_NO_MATCHING_RNW = 2;
const EXITCODE_UNSUPPORTED_VERION_RN = 3;
const EXITCODE_USER_CANCEL = 4;

function REACT_NATIVE_WINDOWS_GENERATE_PATH() {
  return require.resolve('react-native-windows/local-cli/generate-windows.js', {
    paths: [process.cwd()],
  });
}

function getReactNativeAppName() {
  console.log('Reading application name from package.json...');
  let name = JSON.parse(fs.readFileSync('package.json', 'utf8')).name;
  if (!name) {
    if (fs.existsSync('app.json')) {
      console.log('Reading application name from app.json...');
      name = JSON.parse(fs.readFileSync('app.json', 'utf8')).name;
    }
  }
  if (!name) {
    console.error('Please specify name in package.json or app.json');
  }
  return name;
}

function getReactNativeVersion() {
  console.log('Reading react-native version from node_modules...');
  const rnPkgJsonPath = require.resolve('react-native/package.json', {
    paths: [process.cwd()],
  });
  if (fs.existsSync(rnPkgJsonPath)) {
    return require(rnPkgJsonPath).version;
  }

  console.error(
    'Error: Must be run from a project that already depends on react-native, and has react-native installed.',
  );
  process.exit(1);
}

function ErrorOutOnUnsupportedVersionOfReactNative(rnVersion: string) {
  console.error(`Error: Unsupported version of react-native: ${rnVersion}`);
  process.exit(EXITCODE_UNSUPPORTED_VERION_RN);
}

function getDefaultReactNativeWindowsSemVerForReactNativeVersion(
  rnVersion: string,
) {
  const validVersion = semver.valid(rnVersion);
  if (validVersion) {
    const major = semver.major(validVersion);
    const minor = semver.minor(validVersion);
    if (major === 0 && minor >= 60) {
      return `^${major}.${minor}.0-0`;
    }
  }
  ErrorOutOnUnsupportedVersionOfReactNative(rnVersion);
}

function getMatchingReactNativeSemVerForReactNativeWindowsVersion(
  rnwVersion: string,
) {
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

function getLatestMatchingVersion(
  pkg: string,
  versionSemVer: string,
): Promise<string> {
  return new Promise((resolve, reject) => {
    if (semver.validRange(versionSemVer)) {
      npm.packages.releases(
        pkg,
        (err: any, details: {[key: string]: object}) => {
          if (err) {
            reject(err);
          } else if (details) {
            const versions = Object.keys(details);
            if (versions.length > 0) {
              const candidates = versions
                .filter(_ => semver.satisfies(_, versionSemVer))
                .sort(semver.rcompare);
              if (candidates && candidates.length > 0) {
                resolve(candidates[0]);
                return;
              }
            }
          }
          reject(
            new Error(`No matching version of ${pkg}@${versionSemVer} found`),
          );
        },
      );
    } else {
      // Assume that versionSemVer is actually a tag
      npm.packages.release(
        pkg,
        versionSemVer,
        (err: any, details: {version: string}[]) => {
          if (err) {
            reject(err);
          } else if (details && details.length > 0) {
            resolve(details[0].version);
            return;
          }
          reject(
            new Error(`No matching version of ${pkg}@${versionSemVer} found`),
          );
        },
      );
    }
  });
}

async function getLatestMatchingRNWVersion(
  versionSemVer: string,
): Promise<string> {
  try {
    const version = await getLatestMatchingVersion(
      'react-native-windows',
      versionSemVer,
    );
    if (!version) {
      console.error(
        `Error: No version of react-native-windows@${versionSemVer} found`,
      );
      process.exit(EXITCODE_NO_MATCHING_RNW);
    }
    return version;
  } catch (err) {
    console.error(
      `Error: No version of react-native-windows@${versionSemVer} found`,
    );
    process.exit(EXITCODE_NO_MATCHING_RNW);
  }
}

/**
 * Check if project is using Yarn (has `yarn.lock` in the tree)
 */
function isProjectUsingYarn(cwd: string) {
  return findUp.sync('yarn.lock', {cwd});
}

async function doMain() {
  try {
    const name = getReactNativeAppName();
    const ns = argv.namespace || name;
    let version = argv.version;

    if (!version) {
      const rnVersion = getReactNativeVersion();
      version = getDefaultReactNativeWindowsSemVerForReactNativeVersion(
        rnVersion,
      );
    }

    let rnwResolvedVersion = await getLatestMatchingRNWVersion(version);

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

      if (semver.prerelease(rnwResolvedVersion)) {
        let rnwLatestVersion = await getLatestMatchingRNWVersion('latest');
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

        const confirm = (await prompts({
          type: 'confirm',
          name: 'confirm',
          message: `Do you wish to continue with ${chalk.green(
            'react-native-windows',
          )}@${chalk.cyan(rnwResolvedVersion)}?`,
        })).confirm;

        if (!confirm) {
          process.exit(EXITCODE_USER_CANCEL);
        }
      }
    }

    const pkgmgr = isProjectUsingYarn(process.cwd())
      ? 'yarn add'
      : 'npm install --save';

    const execOptions = argv.verbose ? {stdio: 'inherit' as 'inherit'} : {};
    console.log(
      `Installing ${chalk.green('react-native-windows')}@${chalk.cyan(
        version,
      )}...`,
    );
    execSync(`${pkgmgr} "react-native-windows@${version}"`, execOptions);
    console.log(
      chalk.green(`react-native-windows@${version} successfully installed.`),
    );

    const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
    generateWindows(process.cwd(), name, ns, {
      language: argv.language,
      overwrite: argv.overwrite,
      verbose: argv.verbose,
    });
  } catch (error) {
    console.error(chalk.red(error.message));
    console.error(error);
    process.exit(1);
  }
}

doMain();
