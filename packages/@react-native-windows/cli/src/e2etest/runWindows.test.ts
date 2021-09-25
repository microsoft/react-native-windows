/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {runWindowsOptions} from '../runWindows/runWindowsOptions';

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
  }
});
