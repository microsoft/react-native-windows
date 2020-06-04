/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as FileSearch from './FileSearch';
import * as Serialized from './Serialized';

import * as chalk from 'chalk';
import * as fs from 'fs';
import * as ora from 'ora';
import * as path from 'path';
import * as yargs from 'yargs';

import OverrideFactory, {OverrideFactoryImpl} from './OverrideFactory';
import {
  OverrideFileRepository,
  ReactFileRepository,
  bindVersion,
} from './FileRepository';
import {getInstalledRNVersion, getNpmPackage} from './PackageUtils';
import {overrideFromDetails, promptForOverrideDetails} from './OverridePrompt';

import CrossProcessLock from './CrossProcessLock';
import GitReactFileRepository from './GitReactFileRepository';
import Manifest from './Manifest';
import OverrideFileRepositoryImpl from './OverrideFileRepositoryImpl';
import {UpgradeResult} from './UpgradeStrategy';
import {ValidationError} from './ValidationStrategy';

const npmPackage = getNpmPackage();

doMain(() => {
  return new Promise((resolve, _) => {
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
        cmdArgv => validateManifests(cmdArgv.manifests!, cmdArgv.version),
      )
      .command(
        'add <override>',
        'Add an override to the manifest',
        cmdYargs =>
          cmdYargs.options({
            override: {type: 'string', describe: 'The override to add'},
          }),
        cmdArgv => addOverride(cmdArgv.override!),
      )
      .command(
        'remove <override>',
        'Remove an override from the manifest',
        cmdYargs =>
          cmdYargs.options({
            override: {type: 'string', describe: 'The override to remove'},
          }),
        cmdArgv => removeOverride(cmdArgv.override!),
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
        cmdArgv => autoUpgrade(cmdArgv.manifest!, cmdArgv.version),
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
        cmdArgv => manualUpgrade(cmdArgv.manifest!, cmdArgv.version),
      )
      .epilogue(npmPackage.description)
      .option('color', {hidden: true})
      .demandCommand()
      .recommendCommands()
      .strict()
      .showHelpOnFail(false)
      .wrap(yargs.terminalWidth())
      .version(false)
      .scriptName(npmPackage.name)
      .onFinishCommand(resolve).argv;
  });
});

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
    await validateManifest(manifestPath, version);
  }
}

async function validateManifest(manifestPath: string, version?: string) {
  const fullPath = path.resolve(manifestPath);
  const spinner = ora(`Verifying overrides in ${fullPath}`).start();

  await spinnerGuard(spinner, async () => {
    const {manifest, overrideRepo, reactRepo} = await createManifestContext(
      manifestPath,
      version,
    );

    const validationErrors = await manifest.validate(overrideRepo, reactRepo);

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
  const {manifest, overrideFactory} = await createManifestContext(manifestPath);

  const manifestDir = path.dirname(manifestPath);
  const overrideName = path.normalize(path.relative(manifestDir, overridePath));

  if (manifest.hasOverride(overrideName)) {
    console.warn(
      chalk.yellow(
        'Warning: override already exists in manifest and will be overwritten',
      ),
    );
    manifest.removeOverride(overrideName);
  }

  const promptAnswers = await promptForOverrideDetails();

  const spinner = ora(
    'Adding override (This may take a while on first run)',
  ).start();
  await spinnerGuard(spinner, async () => {
    const override = await overrideFromDetails(
      overrideName,
      promptAnswers,
      overrideFactory,
    );
    manifest.addOverride(override);

    await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
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
  const overrideName = path.normalize(path.relative(manifestDir, overridePath));

  if (!manifest.removeOverride(overrideName)) {
    console.error(
      chalk.red('Could not remove override. Is it part of the manifest?'),
    );
    process.exit(1);
  }

  await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
  console.log(chalk.greenBright('Override successfully removed'));
}

/**
 * Attempts to automatically merge changes from the current version into
 * out-of-date overrides.
 */
async function autoUpgrade(manifestPath: string, version?: string) {
  return performUpgrade(
    manifestPath,
    false /*allowConflicts*/,
    version || (await getInstalledRNVersion()),
  );
}

/**
 * Similar to auto-upgrade, but places conflict markers in files that cannot be
 * automatically merged.
 */
async function manualUpgrade(manifestPath: string, version?: string) {
  return performUpgrade(
    manifestPath,
    true /*allowConflicts*/,
    version || (await getInstalledRNVersion()),
  );
}

/**
 * Helper for autoUpgrade and manualUpgrade
 */
async function performUpgrade(
  manifestPath: string,
  allowConflicts: boolean,
  version: string,
) {
  const ctx = await createManifestContext(manifestPath, version);

  const spinner = ora('Merging overrides').start();
  await spinnerGuard(spinner, async () => {
    const outOfDateOverrides = (await ctx.manifest.validate(
      ctx.overrideRepo,
      ctx.reactRepo,
    ))
      .filter(err => err.type === 'outOfDate')
      .map(err => err.overrideName)
      .map(ovrName => ctx.manifest.findOverride(ovrName)!);

    const upgradeResults: Array<UpgradeResult> = [];

    let i = 0;
    for (const override of outOfDateOverrides) {
      spinner.text = `Merging overrides (${++i}/${outOfDateOverrides.length})`;

      const upgradeResult = await override
        .upgradeStrategy()
        .upgrade(ctx.gitReactRepo, ctx.overrideRepo, version, allowConflicts);

      upgradeResults.push(upgradeResult);

      if (allowConflicts || !upgradeResult.hasConflicts) {
        await ctx.manifest.markUpToDate(override.name(), ctx.overrideFactory);
      }
    }

    if (upgradeResults.length > 0) {
      await Serialized.writeManifestToFile(
        ctx.manifest.serialize(),
        manifestPath,
      );
    }

    spinner.succeed();
    printUpgradeStats(upgradeResults, allowConflicts);
  });
}

/**
 * Print statistics about an attempt to upgrade out-of-date-overrides.
 */
function printUpgradeStats(
  results: Array<UpgradeResult>,
  allowConflicts: boolean,
) {
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
  if (allowConflicts && numConflicts > 0) {
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

  printErrorType(
    'missingFromManifest',
    errors,
    `Found override files that aren't listed in the manifest. Overrides can be added to the manifest by using 'npx ${
      npmPackage.name
    } add <override>':`,
  );

  printErrorType(
    'overrideNotFound',
    errors,
    `Found overrides in the manifest that don't exist on disk. Remove existing overrides using 'npx ${
      npmPackage.name
    } remove <override>':`,
  );

  printErrorType(
    'baseNotFound',
    errors,
    `Found overrides whose base files do not exist. Remove existing overrides using 'npx ${
      npmPackage.name
    } remove <override>':`,
  );

  printErrorType(
    'outOfDate',
    errors,
    `Found overrides whose original files have changed. Upgrade overrides using 'npx ${
      npmPackage.name
    } auto-upgrade <manifest>' and 'npx ${
      npmPackage.name
    } manual-upgrade <manifest>':`,
  );

  printErrorType(
    'overrideDifferentFromBase',
    errors,
    'The following overrides should be an exact copy of their base files. Ensure overrides are up to date or revert changes:',
  );
}

/**
 * Print validation errors of a speccific type
 */
function printErrorType(
  type: ValidationError['type'],
  errors: ValidationError[],
  message: string,
) {
  const filteredErrors = errors.filter(err => err.type === type);
  filteredErrors.sort((a, b) => a.overrideName.localeCompare(b.overrideName));

  if (filteredErrors.length > 0) {
    console.error(chalk.red(message));
    filteredErrors.forEach(err => console.error(` - ${err.overrideName}`));
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
async function readManifest(file: string): Promise<Manifest> {
  try {
    const data = await Serialized.readManifestFromFile(file);
    return Manifest.fromSerialized(data);
  } catch (ex) {
    throw new Error('Could not parse manifest. Is it valid?');
  }
}

/**
 * Context describing state centered around a single manifest
 */
interface ManifestContext {
  overrideRepo: OverrideFileRepository;
  reactRepo: ReactFileRepository;
  gitReactRepo: GitReactFileRepository;
  overrideFactory: OverrideFactory;
  manifest: Manifest;
}

/**
 * Sets up state for a manifest describing overrides at a specified RN version
 */
async function createManifestContext(
  manifestPath: string,
  version?: string,
): Promise<ManifestContext> {
  await checkFileExists('manifest', manifestPath);
  version = version || (await getInstalledRNVersion());

  const overrideDir = path.dirname(manifestPath);
  const overrideRepo = new OverrideFileRepositoryImpl(overrideDir);

  const gitReactRepo = await GitReactFileRepository.createAndInit();
  const reactRepo = bindVersion(gitReactRepo, version);

  const overrideFactory = new OverrideFactoryImpl(reactRepo, overrideRepo);
  const manifest = await readManifest(manifestPath);

  return {overrideRepo, reactRepo, gitReactRepo, overrideFactory, manifest};
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
 * Wrap the main function around a barrier to ensure only one copy of the
 * override tool is running at once. This is needed to avoid multiple tools
 * accessing the same local Git repo at the same time.
 */
async function doMain(fn: () => Promise<void>): Promise<void> {
  const lock = new CrossProcessLock(`${npmPackage.name}-cli-lock`);

  if (!(await lock.tryLock())) {
    const spinner = ora(
      'Waiting for other instances of the override CLI to finish',
    ).start();
    await lock.lock();
    spinner.stop();
  }

  await fn();
  lock.unlock();
}
