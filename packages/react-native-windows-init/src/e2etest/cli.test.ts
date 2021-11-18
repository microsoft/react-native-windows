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
    case 'useHermes':
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
  for (const optionName of Object.keys(windowsInitOptions)) {
    const option = windowsInitOptions[optionName];

    // Validate type
    expect(option.type).toBeDefined();

    // Validate defaults
    expect(option).toHaveProperty('default');

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
