/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import chalk from 'chalk';
import {performance} from 'perf_hooks';
import {Ora} from 'ora';

import type {Command, Config} from '@react-native-community/cli-types';
import {Telemetry, CodedError} from '@react-native-windows/telemetry';
import type {
  CodeGenOptions as RnwCodeGenOptions,
  CppStringTypes,
} from '@react-native-windows/codegen';
import {runCodeGen} from '@react-native-windows/codegen';

import {
  newSpinner,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import type {CodeGenOptions} from './codegenWindowsOptions';
import {codegenOptions} from './codegenWindowsOptions';

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

    let cppStringType: CppStringTypes = 'std::string';
    if (pkgJson.codegenConfig.windows.cppStringType) {
      switch (pkgJson.codegenConfig.windows.cppStringType) {
        case 'std::string':
        case 'std::wstring':
          cppStringType = pkgJson.codegenConfig.windows.cppStringType;
          break;
        default:
          throw new CodedError(
            'InvalidCodegenConfig',
            `Value of ${chalk.bold(
              'codegenConfig.windows.cppStringType',
            )} in package.json should be either 'std::string' or 'std::wstring'`,
          );
      }
    }

    let separateDataTypes = false;
    if (pkgJson.codegenConfig.windows.separateDataTypes !== undefined) {
      switch (pkgJson.codegenConfig.windows.separateDataTypes) {
        case true:
        case false:
          separateDataTypes = pkgJson.codegenConfig.windows.separateDataTypes;
          break;
        default:
          throw new CodedError(
            'InvalidCodegenConfig',
            `Value of ${chalk.bold(
              'codegenConfig.windows.separateDataTypes',
            )} in package.json should be either true or false`,
          );
      }
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
    const codegenOutputDir =
      pkgJson.codegenConfig.windows.outputDirectory ?? 'codegen';

    const generators = pkgJson.codegenConfig.windows.generators ?? [
      'modulesWindows',
    ];

    const jsRootPathRelative = path
      .relative(process.cwd(), jsRootDir)
      .split(path.sep)
      .join('/');
    const options: RnwCodeGenOptions = {
      files: [
        `${jsRootPathRelative}${
          jsRootPathRelative ? '/' : ''
        }**/*Native*.[jt]s`,
      ],
      componentsWindows: generators.indexOf('componentsWindows') !== -1,
      internalComponents: generators.indexOf('internalComponents') !== -1,
      cppStringType,
      separateDataTypes,
      libraryName: projectName,
      methodOnly: false,
      modulesCxx: generators.indexOf('modulesCxx') !== -1,
      modulesTypeScriptTypes:
        generators.indexOf('modulesTypeScriptTypes') !== -1,
      modulesWindows: generators.indexOf('modulesWindows') !== -1,
      namespace: projectNamespace,
      outputDirectory: path.join(this.root, codegenOutputDir),
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
 * Sanitizes the given option for telemetry.
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
 * The function run when calling `npx @react-native-community/cli codegen-windows`.
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
      const codegenCommand = 'npx @react-native-community/cli codegen-windows';
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

/**
 * Performs codegen for RNW native modules.
 */
export const codegenCommand: Command = {
  name: 'codegen-windows',
  description: 'Runs Windows-specific codegen for native modules',
  func: codegenWindows,
  options: codegenOptions,
};
