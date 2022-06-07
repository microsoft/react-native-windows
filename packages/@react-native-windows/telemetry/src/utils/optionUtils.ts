/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {camelCase, kebabCase} from 'lodash';

export type OptionSanitizer = (key: string, value: any) => any;

export type YargsOptionsType = Record<string, any>;

export interface CommanderOptionsType extends Record<string, any> {
  logging?: boolean;
  telemetry?: boolean;
}

/**
 * Converts a raw Commander arg name ('--no-bad-dog') into an options property name ('badDog').
 * @param name The raw Commander arg name.
 * @returns The options name.
 */
export function commanderNameToOptionName(name: string): string {
  if (name.startsWith('--')) {
    name = name.slice('--'.length);
  }
  if (name.startsWith('no-')) {
    name = name.slice('no-'.length);
  }
  if (name.endsWith(' [string]')) {
    name = name.slice(0, name.length - ' [string]'.length);
  }
  if (name.endsWith(' [number]')) {
    name = name.slice(0, name.length - ' [number]'.length);
  }
  return camelCase(name);
}

/**
 * Converts a yargs-parsed options object into an options object suitable for telemetry.
 * @param options The options object as parsed by yargs.
 * @param sanitizer Function to sanitize the option values for telemetry.
 * @returns An options object suitable for telemetry.
 */
export function yargsOptionsToOptions(
  options: YargsOptionsType,
  sanitizer?: OptionSanitizer,
): Record<string, any> {
  const result: Record<string, any> = {};
  for (const key of Object.keys(options)) {
    if (
      !(key in Object.prototype) &&
      key !== '$0' &&
      key !== '_' &&
      key.includes('-') !== camelCase(key) in options
    ) {
      result[key] = sanitizer ? sanitizer(key, options[key]) : options[key];
    }
  }
  return result;
}

/**
 * Converts a Commander-parsed options object into an options object suitable for telemetry.
 * @param options The options object as parsed by Commander.
 * @param sanitizer Function to sanitize the option values for telemetry.
 * @returns An options object suitable for telemetry.
 */
export function commanderOptionsToOptions(
  options: CommanderOptionsType,
  sanitizer?: OptionSanitizer,
): Record<string, any> {
  const result: Record<string, any> = {};
  for (const key of Object.keys(options)) {
    if (!(key in Object.prototype)) {
      result[key] = sanitizer ? sanitizer(key, options[key]) : options[key];
    }
  }
  return result;
}

/**
 * Checks whether the given command-line args contains the given option key.
 * @param key The option key to look for.
 * @param argv The raw command-line args.
 * @returns Whether the given command-line args contains the given option key.
 */
export function argsContainsOption(key: string, argv: string[]): boolean {
  const kebabKey = kebabCase(key);
  return (
    argv.includes(`--${key}`) ||
    argv.includes(`--no-${key}`) ||
    argv.includes(`--${kebabKey}`) ||
    argv.includes(`--no-${kebabKey}`)
  );
}

/**
 * Filters down a given set of options to only include those that were present in the command-line args.
 * @param options The full set of options.
 * @param argv The raw command-line args.
 * @returns The filtered set of options.
 */
export function optionsToArgs(
  options: Record<string, any>,
  argv: string[],
): Record<string, any> {
  const result: Record<string, any> = {};
  for (const key of Object.keys(options)) {
    if (!(key in Object.prototype) && argsContainsOption(key, argv)) {
      result[key] = options[key];
    }
  }
  return result;
}
