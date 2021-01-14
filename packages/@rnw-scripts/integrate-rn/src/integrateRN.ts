/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';
import * as semver from 'semver';
import * as yargs from 'yargs';

import {
  findPackage,
  enumerateRepoPackages,
  NpmPackage,
  WritableNpmPackage,
} from '@react-native-windows/package-utils';

import {
  upgradeOverrides,
  validateManifest,
  ValidationError,
} from 'react-native-platform-override';

import runCommand from './runCommand';
import upgradeDependencies from './upgradeDependencies';
import {Logger, CompositeLogger, ConsoleLogger, MarkdownLogger} from './Logger';

let logger: Logger;

(async () => {
  const {argv} = yargs
    .options({
      reportPath: {
        type: 'string',
        describe: 'Optional path to generate a markdown output of results',
        demandOption: false,
      },
    })
    .check(args => {
      if (args._.length === 1 && semver.valid(<string>args._[0])) {
        return true;
      } else {
        throw new Error('Usage: integrate-rn <version>');
      }
    })
    .showHelpOnFail(false);

  const version = <string>argv._[0];

  logger = new CompositeLogger([
    new ConsoleLogger(process.stdout),
    ...(argv.reportPath
      ? [new MarkdownLogger(fs.createWriteStream(argv.reportPath))]
      : []),
  ]);
  try {
    await performSteps(version);
  } finally {
    logger.close();
  }
})();

async function performSteps(newVersion: string) {
  logger.info(`Commits: ${await generateCommitsUrl(newVersion)}`);

  await funcStep(
    `Updating packages and dependants to react-native@${newVersion}`,
    async () => {
      await upgradeDependencies(newVersion);
      return {status: 'success'};
    },
  );

  await funcStep('Upgrading out-of-date overrides', upgradePlatformOverrides);
  await funcStep(
    'Performing additional override validation',
    validatePlatformOverrides,
  );

  await failableCommandStep('yarn lint:fix');
  await commandStep('yarn build');
  await failableCommandStep('yarn lint');
}

/**
 * Enumerate packages subject to override validation
 */
async function enumerateOverridePackages(): Promise<WritableNpmPackage[]> {
  return await enumerateRepoPackages(isOverridePackage);
}

/**
 * Whether the NPM package is subject to override validation
 */
async function isOverridePackage(pkg: NpmPackage): Promise<boolean> {
  try {
    await fs.promises.access(path.join(pkg.path, 'overrides.json'));
    return true;
  } catch {
    return false;
  }
}

/**
 * Upgrade platform overrides in the repo to the current version of react
 * native, disallowing files with conflicts to be written
 */
async function upgradePlatformOverrides(): Promise<StepResult> {
  const overridesWithConflicts: string[] = [];

  for (const pkg of await enumerateOverridePackages()) {
    const results = await upgradeOverrides(
      path.join(pkg.path, 'overrides.json'),
      {allowConflicts: false},
    );

    overridesWithConflicts.push(
      ...results
        .filter(r => r.hasConflicts)
        .map(r => r.overrideName)
        .map(o => path.join(pkg.path, o)),
    );
  }

  if (overridesWithConflicts.length === 0) {
    return {status: 'success'};
  } else {
    return {
      status: 'warn',
      body: `Conflicts detected in the following files. Use 'react-native-platform-override upgrade' to resolve: \n- ${overridesWithConflicts.join(
        '\n- ',
      )}`,
    };
  }
}

/**
 * Perform additional validation on platform overrides apart from the previous
 * up-to-date check
 */
async function validatePlatformOverrides(): Promise<StepResult> {
  const errors: ValidationError[] = [];

  for (const pkg of await enumerateOverridePackages()) {
    errors.push(
      ...(await validateManifest(path.join(pkg.path, 'overrides.json'))),
    );
  }

  if (errors.filter(e => e.type !== 'outOfDate').length !== 0) {
    return {
      status: 'warn',
      body:
        'Override validation failed. Run `yarn validate-overrides` for more information',
    };
  } else {
    return {status: 'success'};
  }
}

/**
 * Creates a URL to show commits that have ocurred between the current RN
 * version and the new version
 */
async function generateCommitsUrl(newRnVersion: string): Promise<string> {
  const rnwPackage = (await findPackage('react-native-windows'))!;
  const rnPackage = (await findPackage('react-native', {
    searchPath: rnwPackage.path,
  }))!;

  return `https://github.com/facebook/react-native/compare/${refFromVersion(
    rnPackage.json.version,
  )}...${refFromVersion(newRnVersion)}`;
}

/**
 * Returns a git ref that may be used in a GitHub comparison URL from an npm package version
 */
function refFromVersion(reactNativeVersion: string): string {
  if (!semver.valid(reactNativeVersion)) {
    throw new Error(`${reactNativeVersion} is not a valid semver version`);
  }

  // Stable releases of React Native use a tag where nightly releases embed
  // a commit hash into the prerelease tag of 0.0.0 versions
  if (semver.lt(reactNativeVersion, '0.0.0', {includePrerelease: true})) {
    return semver.prerelease(reactNativeVersion)![0];
  } else {
    return `v${reactNativeVersion}`;
  }
}

type StepResult = {
  status: 'success' | 'warn' | 'error';
  body?: string;
};

/**
 * Run the function while showing a spinner
 */
async function funcStep(
  name: string,
  func: () => Promise<StepResult>,
): Promise<void> {
  logger.newTask(name);
  try {
    const result = await func();
    logger[result.status](name, result.body);
  } catch (ex) {
    logger.error(name, ex.stack);
    throw ex;
  }
}

/**
 * Run the command while showing a spinner
 */
async function commandStep(cmd: string) {
  return funcStep(cmd, async () => {
    await runCommand(cmd);
    return {status: 'success'};
  });
}

/**
 * Run the command while showing a spinner. Continue if the command returns a non-zero exit code.
 */
async function failableCommandStep(cmd: string) {
  logger.newTask(cmd);
  try {
    await runCommand(cmd);
    logger.success(cmd);
  } catch (ex) {
    logger.error(cmd, ex.stack);
  }
}
