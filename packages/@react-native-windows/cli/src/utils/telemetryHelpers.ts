/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {camelCase} from 'lodash';

import type {
  Config,
  CommandOption,
  OptionValue,
} from '@react-native-community/cli-types';

import {
  Telemetry,
  CodedError,
  CommandStartInfo,
  CommandEndInfo,
  CommanderOptionsType,
  commanderOptionsToOptions,
  optionsToArgs,
  OptionSanitizer,
  configToProjectInfo,
  getProjectFileFromConfig,
  deviceArchitecture,
  nodeArchitecture,
} from '@react-native-windows/telemetry';

import {newInfo, newWarn} from './commandWithProgress';

/**
 * Calculates a the default values of a given react-native CLI command's options.
 * @param config Config passed from react-native CLI.
 * @param commandOptions The options definition for the command.
 * @returns The default options for the command.
 */
export function getDefaultOptions(
  config: Config,
  commandOptions: CommandOption[],
): CommanderOptionsType {
  const result: CommanderOptionsType = {
    logging: false,
  };
  for (const option of commandOptions) {
    let key = option.name.startsWith('--')
      ? option.name.slice('--'.length)
      : option.name;
    key = key.trim();

    let value = option.default;
    if (value !== undefined && (value as (ctx: Config) => OptionValue).name) {
      value = (value as (ctx: Config) => OptionValue)(config);
    }

    if (key.endsWith(' [string]')) {
      // Option is a string
      key = key.slice(0, -' [string]'.length);
      // Option is a number
    } else if (key.endsWith(' [number]')) {
      key = key.slice(0, -' [number]'.length);
    } else {
      // Option is a boolean
      // Note: Commander ignores the default property for booleans
      if (key.startsWith('no-')) {
        key = key.slice('no-'.length);
        // Commander always defaults to true for flags that start with --no
        value = true;
      } else {
        // Commander always defaults to undefined for flags
        value = undefined;
      }
    }

    key = camelCase(key);
    result[key] = value;
  }
  return result;
}

/**
 * Sets up and starts the telemetry gathering for the CLI command.
 * @param commandName The name of the CLI command.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 * @param defaultOptions Default options for the command.
 * @param optionSanitizer Function to sanitize the option values for telemetry.
 */
export async function startTelemetrySession(
  commandName: string,
  config: Config,
  options: CommanderOptionsType,
  defaultOptions: CommanderOptionsType,
  optionSanitizer: OptionSanitizer,
) {
  const verbose = options.logging === true;

  if (!options.telemetry) {
    if (verbose) {
      newInfo('Telemetry is disabled');
    }
    return;
  }

  if (verbose) {
    newInfo(
      `Running ${nodeArchitecture()} node on a ${deviceArchitecture()} machine`,
    );
  }

  if (deviceArchitecture() !== nodeArchitecture()) {
    newWarn(
      'This version of node was built for a different architecture than this machine and may cause unintended behavior',
    );
  }

  await Telemetry.setup();

  const sanitizedOptions = commanderOptionsToOptions(options, optionSanitizer);
  const sanitizedDefaultOptions = commanderOptionsToOptions(
    defaultOptions,
    optionSanitizer,
  );
  const sanitizedArgs = optionsToArgs(sanitizedOptions, process.argv);

  const startInfo: CommandStartInfo = {
    commandName,
    args: sanitizedArgs,
    options: sanitizedOptions,
    defaultOptions: sanitizedDefaultOptions,
  };

  Telemetry.startCommand(startInfo);

  const projectInfo = await configToProjectInfo(config);
  if (projectInfo) {
    Telemetry.setProjectInfo(projectInfo);
  }

  const projectFile = getProjectFileFromConfig(config);
  if (projectFile) {
    await Telemetry.populateNuGetPackageVersions(projectFile);
  }
}

/**
 * Ends the gathering of telemetry for the CLI command.
 * @param error The error (if any) thrown during the command.
 * @param getExtraProps Function to get any extra command-specific telemetry properties.
 */
export async function endTelemetrySession(
  error?: Error,
  getExtraProps?: () => Promise<Record<string, any>>,
) {
  if (!Telemetry.isEnabled()) {
    // Bail early so don't waste time here
    return;
  }

  const endInfo: CommandEndInfo = {
    resultCode: 'Success',
  };

  if (error) {
    endInfo.resultCode =
      error instanceof CodedError ? (error as CodedError).type : 'Unknown';
  }

  Telemetry.endCommand(
    endInfo,
    getExtraProps ? await getExtraProps() : undefined,
  );
}
