/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as ora from 'ora';
import * as path from 'path';
import * as semver from 'semver';
import * as yargs from 'yargs';

import {
  enumerateLocalPackages,
  NpmPackage,
  WritableNpmPackage,
} from '@rnw-scripts/package-utils';

import {
  upgradeOverrides,
  validateManifest,
  ValidationError,
} from 'react-native-platform-override';

import runCommand from './runCommand';
import upgradeDependencies from './upgradeDependencies';

(async () => {
  const {argv} = yargs
    .check(args => {
      if (args._.length === 1 && semver.valid(<string>args._[0])) {
        return true;
      } else {
        throw new Error('Usage: integrate-rn <version>');
      }
    })
    .showHelpOnFail(false);

  const version = <string>argv._[0];

  await funcStep(
    `Updating packages and dependants to react-native@${version}`,
    async () => await upgradeDependencies(version),
  );

  await funcStep('Upgrading out-of-date overrides', upgradePlatformOverrides);
  await funcStep(
    'Performing additional override validation',
    validatePlatformOverrides,
  );

  await failableCommandStep('yarn lint:fix');
  await commandStep('yarn build');
  await failableCommandStep('yarn lint');
})();

/**
 * Enumerate packages subject to override validation
 */
async function enumerateOverridePackages(): Promise<WritableNpmPackage[]> {
  return await enumerateLocalPackages(isOverridePackage);
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
async function upgradePlatformOverrides(spinner: ora.Ora) {
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

  if (overridesWithConflicts.length !== 0) {
    spinner.warn();
    console.warn(
      `Conflicts detected in the following files. Use 'react-native-platform-override upgrade' to resolve: \n- ${overridesWithConflicts.join(
        '\n- ',
      )}`,
    );
  }
}

/**
 * Perform additional validation on platform overrides apart from the previous
 * up-to-date check
 */
async function validatePlatformOverrides(spinner: ora.Ora) {
  const errors: ValidationError[] = [];

  for (const pkg of await enumerateOverridePackages()) {
    errors.push(
      ...(await validateManifest(path.join(pkg.path, 'overrides.json'))),
    );
  }

  if (errors.filter(e => e.type !== 'outOfDate').length !== 0) {
    spinner.warn();
    console.warn(
      "Additional errors detected. Run 'yarn validate-overrides' for more information",
    );
  }
}

/**
 * Run the function while showing a spinner
 */
async function funcStep<T>(
  name: string,
  func: (s: ora.Ora) => Promise<T>,
): Promise<T> {
  const spinner = ora(name).start();
  try {
    const t = await func(spinner);
    if (spinner.isSpinning) {
      spinner.succeed();
    }
    return t;
  } catch (ex) {
    spinner.fail();
    throw ex;
  }
}

/**
 * Run the command while showing a spinner
 */
async function commandStep(cmd: string) {
  return funcStep(cmd, async () => runCommand(cmd));
}

/**
 * Run the command while showing a spinner. Continue if the command returns a non-zero exit code.
 */
async function failableCommandStep(cmd: string) {
  return funcStep(cmd, async spinner => {
    try {
      await runCommand(cmd);
    } catch (ex) {
      spinner.fail();
      console.error(ex.message);
    }
  });
}
