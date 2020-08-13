/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as async from 'async';
import * as fs from 'fs';
import * as ora from 'ora';
import * as path from 'path';
import * as semver from 'semver';
import * as yargs from 'yargs';

import {
  enumerateLocalPackages,
  findPackage,
  NpmPackage,
  WritableNpmPackage,
} from '@rnw-scripts/package-utils';

import {exec, ExecOptions} from 'child_process';
import {
  upgradeOverrides,
  validateManifest,
  ValidationError,
} from 'react-native-platform-override';

import findRepoRoot from '@rnw-scripts/find-repo-root';

(async () => {
  const {argv} = yargs
    .check(args => {
      if (args._.length === 1 && semver.valid(args._[0])) {
        return true;
      } else {
        throw new Error('Usage: integrate-rn <version>');
      }
    })
    .showHelpOnFail(false);

  const version = argv._[0];

  await funcStep(
    `Updating packages and dependants to react-native@${version}`,
    async () => await updatePackages(version),
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
 * Enumerate packages for out-of-tree platforms
 */
async function enumeratePlatformPackages(): Promise<WritableNpmPackage[]> {
  return await enumerateLocalPackages(isOutOfTreePlatform);
}

/**
 * Does the NPM package correspond to an out-of-tree platform?
 */
async function isOutOfTreePlatform(pkg: NpmPackage): Promise<boolean> {
  try {
    await fs.promises.access(path.join(pkg.path, 'overrides.json'));
    return true;
  } catch {
    return false;
  }
}

/**
 * Update all package.json dependencies in the repo to account for a new
 * version of React Native
 */
async function updatePackages(newVersion: string) {
  // Be careful to share package objects since writing to one may invalidate
  // copies
  const localPackages = await enumerateLocalPackages();
  const platformPackages = await async.filter(
    localPackages,
    isOutOfTreePlatform,
  );

  if (platformPackages.length === 0) {
    throw new Error(
      'Expected to find a package for at least 1 out-of-tree platform',
    );
  }

  // Use one of the out-of-tree platforms to locate a version of react-native
  const findRnOpts = {searchPath: platformPackages[0].path};

  // 1. Cache a copy of the previous RN package.json
  const origRnJson = (await findPackage('react-native', findRnOpts))!.json;

  // 2. Update everything to use the new version of RN
  for (const pkg of localPackages) {
    await updateDependency(pkg, 'react-native', newVersion);
  }

  // 3. Install the new RN
  await runCommand('yarn install');

  // 4.Update out of tree platforms with RN dependency changes
  const newRnJson = (await findPackage('react-native', findRnOpts))!.json;
  for (const pkg of platformPackages) {
    await applyRnDependencyChanges(pkg, origRnJson, newRnJson);
  }

  // 5. Update packages to account for any changed peerDependencies (e.g. react)
  for (const pkg of localPackages) {
    for (const [name, version] of Object.entries(newRnJson.peerDependencies)) {
      await updateDependency(pkg, name, version as string);
    }
  }

  // 6. Install everything
  await runCommand('yarn install');
}

/**
 * Update an out-of-tree platform with new changes to react-native dependencies
 *
 * @param pkg package to update
 * @param oldRnJson JSON from previous react-native version
 * @param newRNJson JSON from new react-native-version
 */
async function applyRnDependencyChanges(
  pkg: WritableNpmPackage,
  oldRnJson: any,
  newRNJson: any,
) {
  // Because we host JS from RN core, we need to provide all of the
  // dependencies and peerDependencies declared by react-native. Pick all of
  // them, + any extra dependencies we previously had compared to core (e.g.
  // @react-native-windows/cli)
  const extraDeps = _.difference(
    Object.keys(pkg.json.dependencies),
    oldRnJson.dependencies,
  );
  const newDeps = {
    ..._.pick(pkg.json.dependencies, extraDeps),
    ...newRNJson.dependencies,
  };

  const extraPeerDeps = _.difference(
    Object.keys(pkg.json.peerDependencies),
    oldRnJson.peerDependencies,
  );
  const newPeerDeps = {
    ..._.pick(pkg.json.peerDependencies, extraPeerDeps),
    ...newRNJson.peerDependencies,
  };

  // We can be more relaxed for devDependencies since developer environments
  // differs more and react-native itself cannot depend on these. Just make
  // sure our versions match core where we were matching before.
  const newDevDeps = pkg.json.devDependencies;
  for (const [name, version] of Object.entries(newRNJson.devDependencies)) {
    if (
      newDevDeps[name] &&
      newDevDeps[name] === oldRnJson.devDependencies[name]
    ) {
      newDevDeps[name] = version;
    }
  }

  await pkg.assignProps({
    dependencies: sortKeys(newDeps),
    peerDependencies: sortKeys(newPeerDeps),
    devDependencies: sortKeys(newDevDeps),
  });
}

/**
 * Sort an object by keys
 */
function sortKeys<T>(obj: Record<string, T>): Record<string, T> {
  return _(obj)
    .toPairs()
    .sortBy(0)
    .fromPairs()
    .value();
}

/**
 * Updates a package to bump the listed dependency to a given version
 */
async function updateDependency(
  pkg: WritableNpmPackage,
  depName: string,
  newVersion: string,
) {
  const deps = pkg.json.dependencies;
  const devDeps = pkg.json.devDependencies;
  const peerDeps = pkg.json.peerDependencies;

  if (Object.keys(deps || {}).includes(depName)) {
    deps[depName] = bumpSemver(deps[depName], newVersion);
  }

  if (Object.keys(devDeps || {}).includes(depName)) {
    devDeps[depName] = bumpSemver(devDeps[depName], newVersion);
  }

  // Note that updating our peerDependencies has an effect on restricting users
  // ability to update. E.g. upgrading our repo from RN 0.63.0 to 0.63.2 would
  // change our peerDependency from ^0.63.0 to ^0.63.2, raising the minimum
  // version.
  // This is probably better than the alternative though, since we require at
  // least the version we test against, react-native-windows-init will upgrade
  // projects based on peerDependency, and a mismatch would only show as a
  // warning.
  if (Object.keys(peerDeps || {}).includes(depName)) {
    peerDeps[depName] = bumpSemver(peerDeps[depName], newVersion);
  }

  await pkg.mergeProps({
    dependencies: deps,
    devDependencies: devDeps,
    peerDependencies: peerDeps,
  });
}

/**
 * Updates semver version or range to use a new version, preserving previous
 * range semantics. Only simple cases are supported.
 *
 * @param origVersion the original semver range/value
 * @param newVersion the new value to bump to
 */
function bumpSemver(origVersion: string, newVersion: string): string {
  if (!semver.valid(origVersion) || !semver.validRange(origVersion)) {
    throw new Error(`Unable to bump invalid semver '${origVersion}'`);
  }

  // Semver allows multiple ranges, hypen ranges, star ranges, etc. Don't try
  // to reason about how to bump all of those and just bail if we see them.
  const simpleSemver = /([\^~]?)(\d+\.\d+(\.\d+)?(-\w+\.\d+)?)/;
  if (!simpleSemver.test(origVersion)) {
    throw new Error(`Unable to bump complicated semver '${origVersion}'`);
  }

  if (origVersion.startsWith(`~`) || origVersion.startsWith('^')) {
    return `${origVersion[0]}${newVersion}`;
  } else {
    return newVersion;
  }
}

/**
 * Run a command asyncronously, setting cwd to repo root if none is specified
 */
async function runCommand(
  command: string,
  opts: ExecOptions = {},
): Promise<{stdout: string; stderr: string}> {
  if (!opts.cwd) {
    opts.cwd = await findRepoRoot();
  }

  return new Promise((resolve, reject) => {
    exec(command, opts, (err, stdout, stderr) => {
      if (err) {
        reject(err);
      } else {
        resolve({stdout, stderr});
      }
    });
  });
}

/**
 * Upgrade platform overrides in the repo to the current version of react
 * native, disallowing files with conflicts to be written
 */
async function upgradePlatformOverrides(spinner: ora.Ora) {
  const overridesWithConflicts: string[] = [];

  for (const pkg of await enumeratePlatformPackages()) {
    const results = await upgradeOverrides({
      manifestPath: path.join(pkg.path, 'overrides.json'),
      allowConflicts: false,
    });

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

  for (const pkg of await enumeratePlatformPackages()) {
    errors.push(
      ...(await validateManifest({
        manifestPath: path.join(pkg.path, 'overrides.json'),
      })),
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
