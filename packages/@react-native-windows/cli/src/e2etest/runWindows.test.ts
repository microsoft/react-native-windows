/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {getAnonymizedProjectName} from '../runWindows/runWindows';
import {runWindowsOptions} from '../runWindows/runWindowsOptions';

test('getAnonymizedProjectName - Project Exists', async () => {
  const fooName = await getAnonymizedProjectName(
    `${__dirname}/projects/FooPackage`,
  );
  const barName = await getAnonymizedProjectName(
    `${__dirname}/projects/BarPackage`,
  );

  expect(typeof fooName).toBe('string');
  expect(typeof barName).toBe('string');

  expect(fooName!.length).toBeGreaterThan(0);
  expect(barName!.length).toBeGreaterThan(0);

  expect(fooName).not.toBe(barName);
});

test('getAnonymizedProjectName - Project Doesnt Exist', async () => {
  const emptyPackageName = await getAnonymizedProjectName(
    `${__dirname}/projects/BlankApp`,
  );
  expect(emptyPackageName).toBeNull();
});

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
