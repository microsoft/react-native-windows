/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as FileSearch from './FileSearch';
import * as ManifestData from './ManifestData';
import * as OverridePrompt from './OverridePrompt';

import * as chalk from 'chalk';
import * as fs from 'fs';
import * as ora from 'ora';
import * as path from 'path';
import * as yargs from 'yargs';

import Manifest, {ValidationError} from './Manifest';
import {
  OverrideFileRepository,
  ReactFileRepository,
  bindVersion,
} from './FileRepository';
import OverrideUpgrader, {UpgradeResult} from './OverrideUpgrader';

import {getInstalledRNVersion, getNpmPackage} from './PackageUtils';

import CrossProcessLock from './CrossProcessLock';
import GitReactFileRepository from './GitReactFileRepository';
import OverrideFileRepositoryImpl from './OverrideFileRepositoryImpl';
import PackageReactFileRepository from './PackageReactFileRepository';

const npmPackage = getNpmPackage();

yargs
  .command(
    'validate <manifests...>',
    'Verify that overrides are recorded and up-to-date',
    cmdYargs =>
      cmdYargs.options({
        manifests: {
          type: 'array',
          describe: 'Paths to the override manifests to validate',
        },
        version: {
          type: 'string',
          describe: 'Optional React Native version to check against',
        },
      }),
    cmdArgv => validateManifests(cmdArgv.manifests, cmdArgv.version),
  )
  .command(
    'add <override>',
    'Add an override to the manifest',
    cmdYargs =>
      cmdYargs.options({
        override: {type: 'string', describe: 'The override to add'},
      }),
    cmdArgv => addOverride(cmdArgv.override),
  )
  .command(
    'remove <override>',
    'Remove an override from the manifest',
    cmdYargs =>
      cmdYargs.options({
        override: {type: 'string', describe: 'The override to remove'},
      }),
    cmdArgv => removeOverride(cmdArgv.override),
  )
  .command(
    'auto-upgrade <manifest>',
    'Attempts to automatically merge new changes into out-of-date overrides',
    cmdYargs =>
      cmdYargs.options({
        manifest: {
          type: 'string',
          describe: 'Path to an override manifest',
        },
        version: {
          type: 'string',
          describe: 'Optional React Native version to check against',
        },
      }),
    cmdArgv => autoUpgrade(cmdArgv.manifest, cmdArgv.version),
  )
  .command(
    'manual-upgrade <manifest>',
    'Similar to auto-upgrade, but places conflict markers in files that cannot be automatically merged',
    cmdYargs =>
      cmdYargs.options({
        manifest: {
          type: 'string',
          describe: 'Path to an override manifest',
        },
        version: {
          type: 'string',
          describe: 'Optional React Native version to check against',
        },
      }),
    cmdArgv => manualUpgrade(cmdArgv.manifest, cmdArgv.version),
  )
  .epilogue(npmPackage.description)
  .option('color', {hidden: true})
  .demandCommand()
  .recommendCommands()
  .strict()
  .showHelpOnFail(false)
  .wrap(yargs.terminalWidth())
  .version(false)
  .scriptName(npmPackage.name).argv;

/**
 * Check that the given manifests correctly describe overrides and that all
 * overrides are up to date
 */
async function validateManifests(
  manifestPaths: Array<string | number>,
  version?: string,
) {
  for (const manifestPath of manifestPaths) {
    if (typeof manifestPath !== 'string') {
      throw new Error('manifest arguments must be strings');
    }
    if (versionRequiresGitRepo(version)) {
      await ensureSingleGitUser(() => {
        return validateManifest(manifestPath, version);
      });
    } else {
      await validateManifest(manifestPath, version);
    }
  }
}

async function validateManifest(manifestPath: string, version?: string) {
  const fullPath = path.resolve(manifestPath);
  const spinner = ora(`Verifying overrides in ${fullPath}`).start();

  await spinnerGuard(spinner, async () => {
    const manifest = await readManifest(manifestPath, version);
    const validationErrors = await manifest.validate();

    if (validationErrors.length === 0) {
      spinner.succeed();
    } else {
      spinner.fail();
      printValidationErrors(validationErrors);
      process.exitCode = 1;
    }
  });
}

/**
 * Add an override to the manifest
 */
async function addOverride(overridePath: string) {
  await checkFileExists('override', overridePath);

  const manifestPath = await FileSearch.findManifest(overridePath);
  const manifestDir = path.dirname(manifestPath);

  const manifest = await readManifest(manifestPath);
  const relOverride = path.normalize(path.relative(manifestDir, overridePath));

  if (manifest.hasOverride(relOverride)) {
    console.warn(
      chalk.yellow(
        'Warning: override already exists in manifest and will be overwritten',
      ),
    );
    manifest.removeOverride(relOverride);
  }

  const overrideDetails = await OverridePrompt.askForDetails(relOverride);

  const spinner = ora('Adding override').start();
  await spinnerGuard(spinner, async () => {
    await manifest.addOverride(
      overrideDetails.type,
      relOverride,
      overrideDetails.baseFile,
      overrideDetails.issue,
    );

    const manifestData = manifest.getAsData();
    await ManifestData.writeToFile(manifestData, manifestPath);
    spinner.succeed('Adding override');
  });
}

/**
 * Remove an override from the manifest
 */
async function removeOverride(overridePath: string) {
  const manifestPath = await FileSearch.findManifest(overridePath);
  const manifestDir = path.dirname(manifestPath);

  const manifest = await readManifest(manifestPath);
  const relOverride = path.normalize(path.relative(manifestDir, overridePath));

  if (!manifest.removeOverride(relOverride)) {
    console.error(
      chalk.red('Could not remove override. Is it part of the manifest?'),
    );
    process.exit(1);
  }

  await ManifestData.writeToFile(manifest.getAsData(), manifestPath);
  console.log(chalk.greenBright('Override successfully removed'));
}

/**
 * Attempts to automatically merge changes from the current version into
 * out-of-date overrides.
 */
async function autoUpgrade(manifestPath: string, version?: string) {
  return ensureSingleGitUser(() => {
    return doUpgrade(manifestPath, false /*isManual*/, version);
  });
}

/**
 * Similar to auto-upgrade, but places conflict markers in files that cannot be
 * automatically merged.
 */
async function manualUpgrade(manifestPath: string, version?: string) {
  return ensureSingleGitUser(() => {
    return doUpgrade(manifestPath, true /*isManual*/, version);
  });
}

/**
 * Helper for autoUpgrade and manualUpgrade
 */
async function doUpgrade(
  manifestPath: string,
  isManual: boolean,
  version?: string,
) {
  const spinner = ora('Merging overrides').start();
  await spinnerGuard(spinner, async () => {
    const reactRepo = await GitReactFileRepository.createAndInit();
    const ovrRepo = new OverrideFileRepositoryImpl(path.dirname(manifestPath));
    const upgrader = new OverrideUpgrader(reactRepo, ovrRepo);
    const manifest = await readManifestUsingRepos(
      manifestPath,
      ovrRepo,
      bindVersion(reactRepo, version || getInstalledRNVersion()),
    );

    const outOfDateOverrides = (await manifest.validate())
      .filter(err => err.type === 'outOfDate')
      .map(err => err.file)
      .map(file => manifest.findOverride(file)) as Array<
      ManifestData.NonPlatformEntry
    >;

    const upgradedOverrides: Array<UpgradeResult> = [];

    let i = 0;
    for (const override of outOfDateOverrides) {
      spinner.text = `Merging overrides (${++i}/${outOfDateOverrides.length})`;

      const upgraded = await upgrader.getUpgraded(
        override,
        manifest.currentVersion(),
      );

      if (isManual || !upgraded.hasConflicts) {
        await ovrRepo.setFileContents(upgraded.overrideFile, upgraded.content);
        await manifest.markUpToDate(upgraded.overrideFile);
      }

      upgradedOverrides.push(upgraded);
    }

    if (upgradedOverrides.length > 0) {
      await ManifestData.writeToFile(manifest.getAsData(), manifestPath);
    }

    spinner.succeed();
    printUpgradeStats(upgradedOverrides, isManual);
  });
}

/**
 * Print statistics about an attempt to upgrade out-of-date-overrides.
 */
function printUpgradeStats(results: Array<UpgradeResult>, isManual: boolean) {
  const numTotal = results.length;
  const numConflicts = results.filter(res => res.hasConflicts).length;
  const numAutoPatched = numTotal - numConflicts;

  if (numTotal === 0) {
    console.log(chalk.greenBright('No out-of-date overrides detected'));
  } else {
    console.log(
      chalk.greenBright(
        `${numAutoPatched}/${numTotal} out-of-date overrides automatically merged`,
      ),
    );
  }
  if (isManual && numConflicts > 0) {
    console.log(
      chalk.yellowBright(`${numConflicts} overrides require manual resolution`),
    );
  }
}

/**
 * Prints validation errors in a user-readable form to stderr
 */
function printValidationErrors(errors: Array<ValidationError>) {
  if (errors.length === 0) {
    return;
  }

  // Add an initial line of separation
  console.error();

  errors.sort((a, b) => a.file.localeCompare(b.file));

  const filesMissing = errors.filter(err => {
    return err.type === 'fileMissingFromManifest';
  });
  const overridesMissing = errors.filter(err => {
    return err.type === 'overrideFileNotFound';
  });
  const baseFilesNotFound = errors.filter(err => {
    return err.type === 'baseFileNotFound';
  });
  const outOfDateFiles = errors.filter(err => {
    return err.type === 'outOfDate';
  });

  if (filesMissing.length > 0) {
    const errorMessage = `Found override files that aren't listed in the manifest. Overrides can be added to the manifest by using 'npx ${
      npmPackage.name
    } add <override>':`;
    console.error(chalk.red(errorMessage));
    filesMissing.forEach(err => console.error(` - ${err.file}`));
    console.error();
  }

  if (overridesMissing.length > 0) {
    const errorMessage = `Found overrides in the manifest that don't exist on disk. Remove existing overrides using 'npx ${
      npmPackage.name
    } remove <override>':`;
    console.error(chalk.red(errorMessage));
    overridesMissing.forEach(err => console.error(` - ${err.file}`));
    console.error();
  }

  if (baseFilesNotFound.length > 0) {
    const errorMessage = `Found overrides whose original files do not exist. Remove existing overrides using 'npx ${
      npmPackage.name
    } remove <override>':`;
    console.error(chalk.red(errorMessage));
    baseFilesNotFound.forEach(err => console.error(` - ${err.file}`));
    console.error();
  }

  if (outOfDateFiles.length > 0) {
    const errorMessage = `Found overrides whose original files have changed. Upgrade overrides using 'npx ${
      npmPackage.name
    } auto-upgrade <manifest>' and 'npx ${
      npmPackage.name
    } manual-upgrade <manifest>':`;
    console.error(chalk.red(errorMessage));
    outOfDateFiles.forEach(err => console.error(` - ${err.file}`));
    console.error();
  }
}

/**
 * Throw if a file doesn't exist, printing an error message on the way
 */
async function checkFileExists(friendlyName: string, filePath: string) {
  try {
    await fs.promises.access(filePath);
  } catch (ex) {
    throw new Error(`Could not find ${friendlyName} at path '${filePath}'`);
  }
}

/**
 * Read a manifest and print a pretty error if we can't
 */
async function readManifest(file: string, version?: string): Promise<Manifest> {
  let reactRepo: ReactFileRepository;

  if (versionRequiresGitRepo(version)) {
    const gitRepo = await GitReactFileRepository.createAndInit();
    reactRepo = bindVersion(gitRepo, version);
  } else {
    reactRepo = new PackageReactFileRepository();
  }

  const ovrRepo = new OverrideFileRepositoryImpl(path.dirname(file));
  return readManifestUsingRepos(file, ovrRepo, reactRepo);
}

/**
 * Like {@see readManifest}, but allows providing the file repositories to use.
 */
async function readManifestUsingRepos(
  file: string,
  ovrRepo: OverrideFileRepository,
  reactRepo: ReactFileRepository,
): Promise<Manifest> {
  await checkFileExists('manifest', file);

  let data: ManifestData.Manifest;

  try {
    data = await ManifestData.readFromFile(file);
  } catch (ex) {
    throw new Error('Could not parse manifest. Is it valid?');
  }

  return new Manifest(data, ovrRepo, reactRepo);
}

/**
 * Should we use a scratch Git repo for the requested version
 */
function versionRequiresGitRepo(version?: string) {
  return version && version !== getInstalledRNVersion();
}

/**
 * Wraps the function in a try/catch, failing the spinner if an exception is
 * thrown to allow unmangled output
 */
async function spinnerGuard<T>(
  spinner: ora.Ora,
  fn: () => Promise<T>,
): Promise<T> {
  try {
    return await fn();
  } catch (ex) {
    if (spinner.isSpinning) {
      spinner.fail();
    }
    throw ex;
  }
}

/**
 * Ensure that only a single instance of the application can use the scratch Git Repo
 */
async function ensureSingleGitUser(fn: () => Promise<void>): Promise<void> {
  const lock = new CrossProcessLock(`${npmPackage.name}/scratch-git.lock`);

  if (!(await lock.tryLock())) {
    const spinner = ora(
      `Waiting for another instance of ${npmPackage.name} to finish`,
    ).start();
    await lock.lock();
    spinner.stop();
  }

  await fn();
  lock.unlock();
}
