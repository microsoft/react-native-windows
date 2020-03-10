/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as FileSearch from './FileSearch';
import * as ManifestData from './ManifestData';
import * as OverridePrompt from './OverridePrompt';

import * as _ from 'lodash';
import * as chalk from 'chalk';
import * as fs from 'fs';
import * as ora from 'ora';
import * as path from 'path';
import * as yargs from 'yargs';

import Manifest, {ValidationError} from './Manifest';
import {
  OverrideFileRepository,
  VersionedReactFileRepository,
  bindVersion,
} from './FileRepository';

import CrossProcessLock from './CrossProcessLock';
import GitReactFileRepository from './GitReactFileRepository';
import OverrideFileRepositoryImpl from './OverrideFileRepositoryImpl';
import OverrideUprgader from './OverrideUpgrader';
import {getInstalledRNVersion} from './ReactVersion';

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
      .epilogue(
        'This tool allows managing JavaScript overrides for React Native Windows',
      )
      .option('color', {hidden: true})
      .demandCommand()
      .recommendCommands()
      .strict()
      .showHelpOnFail(false)
      .wrap(yargs.terminalWidth())
      .version(false)
      .scriptName('override')
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

  try {
    const manifest = await readManifest(manifestPath, version);
    const validationErrors = await manifest.validate();

    if (validationErrors.length === 0) {
      spinner.succeed();
    } else {
      spinner.fail();
      printValidationErrors(validationErrors);
      process.exitCode = 1;
    }
  } catch (ex) {
    if (spinner.isSpinning) {
      spinner.fail();
    }
    throw ex;
  }
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
  try {
    await manifest.addOverride(
      overrideDetails.type,
      relOverride,
      overrideDetails.baseFile,
      overrideDetails.issue,
    );

    const manifestData = manifest.getAsData();
    await ManifestData.writeToFile(manifestData, manifestPath);
    spinner.succeed();
  } catch (ex) {
    spinner.fail();
    console.error(chalk.red('Could not add override'));
    throw ex;
  }
}

/**
 * Remove an override from the manifest
 */
async function removeOverride(overridePath: string) {
  await checkFileExists('override', overridePath);

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
  const spinner = ora('Merging overrides').start();

  try {
    const reactRepo = await GitReactFileRepository.createAndInit();
    const ovrRepo = new OverrideFileRepositoryImpl(path.dirname(manifestPath));
    const upgrader = new OverrideUprgader(reactRepo, ovrRepo);
    const manifest = await readManifestUsingRepos(
      manifestPath,
      ovrRepo,
      reactRepo,
      version,
    );

    const upgradeReqs = (await manifest.validate())
      .filter(err => err.type === 'outOfDate')
      .map(err => err.file)
      .map(file => manifest.findOverride(file))
      .map((entry: ManifestData.NonPlatformEntry) => ({
        overrideEntry: entry,
        newVersion: manifest.currentVersion(),
      }));

    for (const upgraded of await upgrader.perform(upgradeReqs)) {
      if (!upgraded.hasConflicts) {
        await ovrRepo.setFileContents(upgraded.overrideFile, upgraded.content);
        manifest.markUpToDate(upgraded.overrideFile);
      }
    }

    //`${mergedFiles}/${numFiles} out-of-date overrides merged`
    spinner.succeed();
  } catch (ex) {
    if (spinner.isSpinning) {
      spinner.fail();
    }
    throw ex;
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
    const errorMessage =
      "Found override files that aren't listed in the manifest. Overrides can be added to the manifest by using 'yarn override add <override>':";
    console.error(chalk.red(errorMessage));
    filesMissing.forEach(err => console.error(` - ${err.file}`));
    console.error();
  }

  if (overridesMissing.length > 0) {
    const errorMessage =
      "Found overrides in the manifest that don't exist on disk. Remove existing overrides using 'yarn override remove <override>':";
    console.error(chalk.red(errorMessage));
    overridesMissing.forEach(err => console.error(` - ${err.file}`));
    console.error();
  }

  if (baseFilesNotFound.length > 0) {
    const errorMessage =
      "Found overrides whose original files do not exist. Remove existing overrides using 'yarn override remove <override>':";
    console.error(chalk.red(errorMessage));
    baseFilesNotFound.forEach(err => console.error(` - ${err.file}`));
    console.error();
  }

  if (outOfDateFiles.length > 0) {
    // TODO: Instruct users to use 'yarn override upgrade' once that exists
    console.error(
      chalk.red('Found overrides whose original files have changed:'),
    );
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
    console.error(
      chalk.red(`Could not find ${friendlyName} at path '${filePath}'`),
    );
    throw ex;
  }
}

/**
 * Read a manifest and print a pretty error if we can't
 */
async function readManifest(file: string, version?: string): Promise<Manifest> {
  const gitReactRepo = await GitReactFileRepository.createAndInit();
  const ovrRepo = new OverrideFileRepositoryImpl(path.dirname(file));

  return readManifestUsingRepos(file, ovrRepo, gitReactRepo, version);
}

/**
 * Like {@see readManifest}, but allows providing the file repositories to use.
 */
async function readManifestUsingRepos(
  file: string,
  ovrRepo: OverrideFileRepository,
  versionedReactRepo: VersionedReactFileRepository,
  version?: string,
): Promise<Manifest> {
  await checkFileExists('manifest', file);

  let data: ManifestData.Manifest;

  try {
    data = await ManifestData.readFromFile(file);
  } catch (ex) {
    console.error(chalk.red('Could not parse manifest. Is it valid?'));
    throw ex;
  }

  const rnVersion = version || (await getInstalledRNVersion(file));
  const reactRepo = bindVersion(versionedReactRepo, rnVersion);

  return new Manifest(data, ovrRepo, reactRepo);
}

/**
 * Wrap the main function around a barrier to ensure only one copy of the
 * override tool is running at once. This is needed to avoid multiple tools
 * accessing the same local Git repo at the same time.
 */
async function doMain(fn: () => Promise<void>): Promise<void> {
  const lock = new CrossProcessLock('react-native-windows-override-cli-lock');

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
