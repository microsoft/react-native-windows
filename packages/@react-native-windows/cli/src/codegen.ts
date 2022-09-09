/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import chalk from 'chalk';
import {performance} from 'perf_hooks';

import {
  newSpinner,
  setExitProcessWithError,
} from './runWindows/utils/commandWithProgress';

import {
  Command,
  CommandOption,
  Config,
} from '@react-native-community/cli-types';
import {Telemetry, CodedError} from '@react-native-windows/telemetry';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from './runWindows/utils/telemetryHelpers';

import {runCodeGen} from '@react-native-windows/codegen';
import {Ora} from 'ora';

export class CodeGenWindows {
  private changesNecessary: boolean;

  public areChangesNeeded() {
    return this.changesNecessary;
  }

  constructor(readonly root: string, readonly options: CodeGenOptions) {
    this.changesNecessary = false;
  }

  public async run(spinner: Ora) {
    const verbose = this.options.logging;

    verboseMessage('', verbose);

    verboseMessage('Loading codegenConfig from package.json');
    const pkgJson = require(path.join(this.root, 'package.json'));

    if (!pkgJson.codegenConfig) {
      spinner.info(
        `No ${chalk.bold(
          'codegenConfig',
        )} specified in package.json - ${chalk.yellow(
          'Skipping codegen-windows',
        )}`,
      );
      return;
    }

    const codegenConfigType = pkgJson.codegenConfig.type;
    if (codegenConfigType !== 'modules' && codegenConfigType !== 'all') {
      spinner.info(
        `${chalk.bold(
          'codegenConfig.type',
        )} in package.json is not ${chalk.bold('modules')} or ${chalk.bold(
          'all',
        )} - ${chalk.yellow('Skipping codegen-windows')}`,
      );
      return;
    }

    if (!pkgJson.codegenConfig.windows) {
      spinner.info(
        `No ${chalk.bold(
          'codegenConfig.windows',
        )} specified in package.json - ${chalk.yellow(
          'Skipping codegen-windows',
        )}`,
      );
      return;
    }

    if (!pkgJson.codegenConfig.windows.namespace) {
      throw new CodedError(
        'InvalidCodegenConfig',
        `Missing ${chalk.bold(
          'codegenConfig.windows.namespace',
        )} value in package.json`,
      );
    }

    if (!pkgJson.codegenConfig.name) {
      throw new CodedError(
        'InvalidCodegenConfig',
        `Missing ${chalk.bold('codegenConfig.name')} value in package.json`,
      );
    }

    const projectName = pkgJson.codegenConfig.name.replace(/[^a-zA-Z]/g, '');
    const projectNamespace = pkgJson.codegenConfig.windows.namespace;
    const jsRootDir = pkgJson.codegenConfig.jsSrcsDir
      ? path.join(this.root, pkgJson.codegenConfig.jsSrcsDir)
      : this.root;

    const jsRootPathRelative = path.relative(process.cwd(), jsRootDir);
    const options = {
      files: [
        `${jsRootPathRelative}${
          jsRootPathRelative ? '/' : ''
        }**/*Native*.[jt]s`,
      ],
      namespace: projectNamespace,
      libraryName: projectName,
      outdir: path.join(this.root, 'codegen'),
      methodonly: false,
      ts: false,
      test: !!this.options.check,
    };

    verboseMessage(
      `Run codegen with options: \n${JSON.stringify(options, null, 2)}`,
      verbose,
    );

    this.changesNecessary = runCodeGen(options);

    spinner.succeed();
  }
}

/**
 * Logs the given message if verbose is True.
 * @param message The message to log.
 * @param verbose Whether or not verbose logging is enabled.
 */
function verboseMessage(message: any, verbose?: boolean) {
  if (verbose) {
    console.log(message);
  }
}

/**
 * Sanitizes the given option for telemetery.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(key: keyof CodeGenOptions, value: any): any {
  // Do not add a default case here.
  // Strings risking PII should just return true if present, false otherwise.
  // All others should return the value (or false if undefined).
  switch (key) {
    case 'logging':
    case 'check':
    case 'telemetry':
      return value === undefined ? false : value; // Return value
  }
}

/**
 * Get the extra props to add to the `codegen-windows` telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling `react-native codegen-windows`.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function codegenWindows(
  args: string[],
  config: Config,
  options: CodeGenOptions,
) {
  await startTelemetrySession(
    'codegen-windows',
    config,
    options,
    getDefaultOptions(config, codegenOptions),
    optionSanitizer,
  );

  let codegenWindowsError: Error | undefined;
  try {
    await codegenWindowsInternal(args, config, options);
  } catch (ex) {
    codegenWindowsError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(codegenWindowsError);
  }

  await endTelemetrySession(codegenWindowsError, getExtraProps);
  setExitProcessWithError(options.logging, codegenWindowsError);
}

/**
 * Performs codegen for RNW native modules and apps.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
export async function codegenWindowsInternal(
  args: string[],
  config: Config,
  options: CodeGenOptions,
) {
  const startTime = performance.now();
  const spinner = newSpinner(
    options.check
      ? 'Checking codegen-windows files...'
      : 'Running codegen-windows...',
  );
  try {
    const codegen = new CodeGenWindows(config.root, options);
    await codegen.run(spinner);
    const endTime = performance.now();

    if (!codegen.areChangesNeeded()) {
      console.log(
        `${chalk.green(
          'Success:',
        )} No codegen-windows changes necessary. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    } else if (options.check) {
      const codegenCommand = 'npx react-native codegen-windows';
      console.log(
        `${chalk.yellow(
          'Warning:',
        )} Codegen-windows changes were necessary but ${chalk.bold(
          '--check',
        )} specified. Run '${chalk.bold(
          `${codegenCommand}`,
        )}' to apply the changes. (${Math.round(endTime - startTime)}ms)`,
      );
      throw new CodedError(
        'NeedCodegen',
        `Codegen-windows changes were necessary but --check was specified. Run '${codegenCommand}' to apply the changes`,
      );
    } else {
      console.log(
        `${chalk.green(
          'Success:',
        )} Codegen-windows changes completed. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    }
  } catch (e) {
    spinner.fail();
    const endTime = performance.now();
    console.log(
      `${chalk.red('Error:')} ${(e as any).toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    throw e;
  }
}

export interface CodeGenOptions {
  logging?: boolean;
  check?: boolean;
  telemetry?: boolean;
}

export const codegenOptions: CommandOption[] = [
  {
    name: '--logging',
    description: 'Verbose output logging',
  },
  {
    name: '--check',
    description: 'Only check whether any codegen files need to change',
  },
  {
    name: '--no-telemetry',
    description:
      'Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
  },
];

/**
 * Performs codegen for RNW native modules.
 */
export const codegenCommand: Command = {
  name: 'codegen-windows',
  description: 'Runs Windows specific codegen',
  func: codegenWindows,
  options: codegenOptions,
};
