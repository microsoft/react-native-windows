/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Api from './Api';

import * as chalk from 'chalk';
import * as ora from 'ora';
import * as path from 'path';
import * as yargs from 'yargs';

import {findAllManifests, findManifest} from './FileSearch';
import {overrideFromDetails, promptForOverrideDetails} from './OverridePrompt';

import CrossProcessLock from './CrossProcessLock';
import GitReactFileRepository from './GitReactFileRepository';
import {UpgradeResult} from './UpgradeStrategy';
import {ValidationError} from './ValidationStrategy';
import {getNpmPackage} from './PackageUtils';

void doMain(async () => {
  const npmPackage = await getNpmPackage();

  return new Promise((resolve, _reject) => {
    yargs
      .command(
        'validate',
        'Verify that overrides are recorded and up-to-date',
        cmdYargs =>
          cmdYargs.options({
            manifest: {
              type: 'string',
              describe: 'Optional path to the override manifest to validate',
            },
            version: {
              type: 'string',
              describe: 'Optional React Native version to check against',
            },
          }),
        // eslint-disable-next-line @typescript-eslint/no-misused-promises
        cmdArgv =>
          validateManifests({
            manifestPath: cmdArgv.manifest,
            reactNativeVersion: cmdArgv.version,
          }),
      )
      .command(
        'add <override>',
        'Add an override to the manifest',
        cmdYargs =>
          cmdYargs.options({
            override: {type: 'string', describe: 'The override to add'},
          }),
        // eslint-disable-next-line @typescript-eslint/no-misused-promises
        cmdArgv => addOverride(cmdArgv.override!),
      )
      .command(
        'remove <override>',
        'Remove an override from the manifest',
        cmdYargs =>
          cmdYargs.options({
            override: {type: 'string', describe: 'The override to remove'},
          }),
        // eslint-disable-next-line @typescript-eslint/no-misused-promises
        cmdArgv => removeOverride(cmdArgv.override!),
      )
      .command(
        'diff <override>',
        'Compares an override to the base file of its current version',
        cmdYargs =>
          cmdYargs.options({
            override: {type: 'string', describe: 'The override to add'},
          }),
        // eslint-disable-next-line @typescript-eslint/no-misused-promises
        cmdArgv => diffOverride(cmdArgv.override!),
      )
      .command(
        'upgrade',
        'Attempts to automatically merge new changes into out-of-date overrides',
        cmdYargs =>
          cmdYargs.options({
            manifest: {
              type: 'string',
              describe: 'Optional path to the override manifests to validate',
            },
            conflicts: {
              type: 'boolean',
              default: true,
              describe: 'Whether to allow merge conflicts to be written',
            },
            version: {
              type: 'string',
              describe: 'Optional React Native version to check against',
            },
          }),
        // eslint-disable-next-line @typescript-eslint/no-misused-promises
        cmdArgv =>
          upgrade({
            manifestPath: cmdArgv.manifest,
            reactNativeVersion: cmdArgv.version,
            allowConflicts: cmdArgv.conflicts,
          }),
      )
      .epilogue(npmPackage.description)
      .option('color', {hidden: true})
      .option('githubToken', {
        description: 'Optional PAT to use for GitHub API calls',
        type: 'string',
      })
      .middleware(argv => {
        if (argv.githubToken) {
          GitReactFileRepository.setGithubToken(argv.githubToken);
        }
      })
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
 * Check that the given manifest correctly describe overrides and that all
 * overrides are up to date
 */
async function validateManifests(opts: {
  manifestPath?: string;
  reactNativeVersion?: string;
}) {
  const manifests = opts.manifestPath
    ? [opts.manifestPath]
    : await enumerateManifests();

  const spinner = ora();
  await spinnerGuard(spinner, async () => {
    // Perform validation sequentially because validation has internal
    // concurrency
    const errors: ValidationError[] = [];
    for (const manifest of manifests) {
      spinner.text = `Validating ${manifest}`;
      spinner.start();

      const manifestErrors = await Api.validateManifest(manifest, opts);
      if (manifestErrors.length !== 0) {
        errors.push(
          // Add the manifest path to the override name to disambiguate between different packages
          ...manifestErrors.map(e => ({
            ...e,
            overrideName: path.join(path.dirname(manifest), e.overrideName),
          })),
        );
      }
    }

    if (errors.length === 0) {
      spinner.text = 'Validation succeeded';
      spinner.succeed();
    } else {
      spinner.text = 'Validation failed';
      spinner.fail();
      await printValidationErrors(errors);
      process.exitCode = 1;
    }
  });
}

/**
 * Add an override to the manifest
 */
async function addOverride(overridePath: string) {
  const manifestPath = await findManifest(path.dirname(overridePath));
  const manifestDir = path.dirname(manifestPath);
  const overrideName = path.relative(manifestDir, path.resolve(overridePath));

  if (await Api.hasOverride(overrideName, manifestPath)) {
    console.warn(
      chalk.yellow(
        'Warning: override already exists in manifest and will be overwritten',
      ),
    );
  }

  const overrideDetails = await promptForOverrideDetails();

  const spinner = ora('Adding override').start();
  await spinnerGuard(spinner, async () => {
    const override = await overrideFromDetails(
      overridePath,
      overrideDetails,
      await Api.getOverrideFactory(manifestPath),
    );

    await Api.removeOverride(overrideName, manifestPath);
    await Api.addOverride(override, manifestPath);
    spinner.succeed();
  });
}

/**
 * Remove an override from the manifest
 */
async function removeOverride(overridePath: string) {
  const manifestPath = await findManifest(path.dirname(overridePath));
  const manifestDir = path.dirname(manifestPath);
  const overrideName = path.relative(manifestDir, path.resolve(overridePath));

  if (await Api.removeOverride(overrideName, manifestPath)) {
    console.log(chalk.greenBright('Override successfully removed'));
  } else {
    console.error(
      chalk.red('Could not remove override. Is it part of the manifest?'),
    );
    process.exit(1);
  }
}

/**
 * Diffs an override against its base file
 */
async function diffOverride(overridePath: string) {
  const manifestPath = await findManifest(path.dirname(overridePath));
  const manifestDir = path.dirname(manifestPath);
  const overrideName = path.relative(manifestDir, path.resolve(overridePath));
  const diff = await Api.diffOverride(overrideName, manifestPath);

  const colorizedDiff = diff
    .split('\n')
    .map(line =>
      line.startsWith('+')
        ? chalk.green(line)
        : line.startsWith('-')
        ? chalk.red(line)
        : line,
    )
    .join('\n');

  console.log(colorizedDiff);
}

/**
 * Attempts to automatically merge changes from the current version into
 * out-of-date overrides.
 */
async function upgrade(opts: {
  manifestPath?: string;
  reactNativeVersion?: string;
  allowConflicts: boolean;
}) {
  const manifests = opts.manifestPath
    ? [opts.manifestPath]
    : await enumerateManifests();

  for (const manifest of manifests) {
    await upgradeManifest(manifest, opts);
  }
}

async function upgradeManifest(
  manifestPath: string,
  opts: {
    reactNativeVersion?: string;
    allowConflicts: boolean;
  },
) {
  const spinner = ora(`Merging overrides in ${manifestPath}`).start();
  await spinnerGuard(spinner, async () => {
    const upgradeResults = await Api.upgradeOverrides(manifestPath, {
      ...opts,
      progressListener: (currentOverride, totalOverrides) =>
        (spinner.text = `Merging overrides in ${manifestPath} (${currentOverride}/${totalOverrides})`),
    });

    spinner.succeed();
    printUpgradeStats(upgradeResults, opts.allowConflicts);
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
async function printValidationErrors(errors: ValidationError[]) {
  if (errors.length === 0) {
    return;
  }

  const npmPackage = await getNpmPackage();

  // Add an initial line of separation
  console.error();

  printErrorType(
    'missingFromManifest',
    errors,
    `Found override files that aren't listed in the manifest. Overrides can be added to the manifest by using 'npx ${npmPackage.name} add <override>':`,
  );

  printErrorType(
    'overrideNotFound',
    errors,
    `Found overrides in the manifest that don't exist on disk. Remove existing overrides using 'npx ${npmPackage.name} remove <override>':`,
  );

  printErrorType(
    'baseNotFound',
    errors,
    `Found overrides whose base files do not exist. Remove existing overrides using 'npx ${npmPackage.name} remove <override>':`,
  );

  printErrorType(
    'outOfDate',
    errors,
    `Found overrides whose original files have changed. Upgrade overrides using 'npx ${npmPackage.name} upgrade':`,
  );

  printErrorType(
    'overrideDifferentFromBase',
    errors,
    'The following overrides should be an exact copy of their base files. Ensure overrides are up to date or revert changes:',
  );

  printErrorType(
    'overrideSameAsBase',
    errors,
    'The following overrides are identical to their base files. Please remove them or set their type to "copy":',
  );

  printErrorType(
    'expectedFile',
    errors,
    'The following overrides should operate on files, but list directories:',
  );

  printErrorType(
    'expectedDirectory',
    errors,
    'The following overrides should operate on directories, but listed files:',
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
  filteredErrors.sort((a, b) =>
    a.overrideName.localeCompare(b.overrideName, 'en'),
  );

  if (filteredErrors.length > 0) {
    console.error(chalk.red(message));
    filteredErrors.forEach(err => {
      console.error(` - ${err.overrideName}`);
    });
    console.error();
  }
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
  const lock = new CrossProcessLock(`${(await getNpmPackage()).name}-cli-lock`);

  if (!(await lock.tryLock())) {
    const spinner = ora(
      'Waiting for other instances of the override CLI to finish',
    ).start();
    await lock.lock();
    spinner.stop();
  }

  await fn();
  await lock.unlock();
}

/**
 * Check that a manifest exists, and return all that are found
 */
async function enumerateManifests(): Promise<string[]> {
  const manifests = await findAllManifests();
  if (manifests.length === 0) {
    throw new Error(
      'No override manifests were found relative to the current directory',
    );
  }

  return manifests;
}
