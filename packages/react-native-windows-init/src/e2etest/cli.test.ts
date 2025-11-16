/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {windowsInitOptions, WindowsInitOptions} from '../Cli';

function validateOptionName(
  name: string,
  optionName: keyof WindowsInitOptions,
): boolean {
  // Do not add a default case here. Every item must explicitly return true
  switch (optionName) {
    case 'version':
    case 'namespace':
    case 'verbose':
    case 'telemetry':
    case 'language':
    case 'overwrite':
    case 'projectType':
    case 'experimentalNuGetDependency':
    case 'useWinUI3':
    case 'nuGetTestVersion':
    case 'nuGetTestFeed':
    case 'useDevMode':
      return true;
  }
  throw new Error(
    `Unable to find ${optionName} to match '${name}' in WindowsInitOptions.`,
  );
}

test('windowsInitOptions - validate options', () => {
  const options = windowsInitOptions as Record<string, any>;
  for (const optionName of Object.keys(options)) {
    const option = options[optionName];

    // Validate type
    expect(option.type).toBeDefined();

    // Validate defaults
    if (option.type === 'string') {
      if (option.choices !== undefined) {
        // If there are choices (enum value) make sure
        // the default is present and of that type
        expect(Array.isArray(option.choices)).toBe(true);
        expect(option).toHaveProperty('default');
        expect(option.default).toBeDefined();
        expect(option.choices.includes(option.default)).toBe(true);
      } else {
        // Regular strings should not have defined default
        expect(option.default).not.toBeDefined();
      }
    } else if (option.conflicts !== undefined) {
      // Options with conflicts defined should have default = undefined
      expect(option).toHaveProperty('default');
      expect(option.default).toBeUndefined();
    } else {
      // Regular options should have defined defaults
      expect(option).toHaveProperty('default');
      expect(option.default).toBeDefined();
    }

    // Validate description
    expect(option.describe).toBeDefined();
    expect(option.describe).not.toBeNull();
    expect(option.describe!).toBe(option.describe!.trim());

    // Validate all command options are present in WindowsInitOptions
    expect(
      validateOptionName(optionName, optionName as keyof WindowsInitOptions),
    ).toBe(true);
  }
});
