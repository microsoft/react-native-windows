/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {commanderNameToOptionName} from '@react-native-windows/telemetry';

import {
  runWindowsOptions,
  RunWindowsOptions,
} from '../commands/runWindows/runWindowsOptions';

// eslint-disable-next-line complexity
function validateOptionName(
  name: string,
  optionName: keyof RunWindowsOptions,
): boolean {
  // Do not add a default case here. Every item must explicitly return true
  switch (optionName) {
    case 'release':
    case 'root':
    case 'arch':
    case 'singleproc':
    case 'emulator':
    case 'device':
    case 'target':
    case 'logging':
    case 'packager':
    case 'bundle':
    case 'launch':
    case 'autolink':
    case 'build':
    case 'deploy':
    case 'deployFromLayout':
    case 'sln':
    case 'proj':
    case 'msbuildprops':
    case 'buildLogDirectory':
    case 'info':
    case 'directDebugging':
    case 'telemetry':
      return true;
  }
  throw new Error(
    `Unable to find ${optionName} to match '${name}' in RunWindowsOptions.`,
  );
}

test('runWindowsOptions - validate options', () => {
  for (const commandOption of runWindowsOptions) {
    // Validate names
    expect(commandOption.name).not.toBeNull();
    expect(commandOption.name.startsWith('--')).toBe(true);
    expect(commandOption.name).toBe(commandOption.name.trim());

    // Validate defaults
    if (
      !commandOption.name.endsWith(' [string]') &&
      !commandOption.name.endsWith(' [number]')
    ) {
      // Commander ignores defaults for flags, so leave undefined to prevent confusion
      expect(commandOption.default).toBeUndefined();
    }

    // Validate description
    expect(commandOption.description).not.toBeNull();
    expect(commandOption.description!).toBe(commandOption.description!.trim());

    // Validate all command options are present in RunWindowsOptions
    const optionName = commanderNameToOptionName(commandOption.name);
    expect(
      validateOptionName(
        commandOption.name,
        optionName as keyof RunWindowsOptions,
      ),
    ).toBe(true);
  }
});
