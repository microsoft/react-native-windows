/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {commanderNameToOptionName} from '@react-native-windows/telemetry';

import {
  initOptions,
  InitOptions,
} from '../commands/initWindows/initWindowsOptions';

import * as nameHelpers from '../utils/nameHelpers';

function validateOptionName(
  name: string,
  optionName: keyof InitOptions,
): boolean {
  // Do not add a default case here. Every item must explicitly return true
  switch (optionName) {
    case 'logging':
    case 'template':
    case 'name':
    case 'namespace':
    case 'overwrite':
    case 'telemetry':
    case 'list':
      return true;
  }
  throw new Error(
    `Unable to find ${optionName} to match '${name}' in InitOptions.`,
  );
}

test('initOptions - validate options', () => {
  for (const commandOption of initOptions) {
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

    // Validate all command options are present in InitOptions
    const optionName = commanderNameToOptionName(commandOption.name);
    expect(
      validateOptionName(commandOption.name, optionName as keyof InitOptions),
    ).toBe(true);
  }
});

test('nameHelpers - cleanName', () => {
  expect(nameHelpers.cleanName('@scope/package')).toBe('Package');
  expect(nameHelpers.cleanName('@scope/package-name')).toBe('PackageName');
  expect(nameHelpers.cleanName('package')).toBe('Package');
  expect(nameHelpers.cleanName('package-name')).toBe('PackageName');
});

test('nameHelpers - isValidProjectName', () => {
  expect(nameHelpers.isValidProjectName('package')).toBe(true);
  expect(nameHelpers.isValidProjectName('package-name')).toBe(false);
  expect(nameHelpers.isValidProjectName('Package')).toBe(true);
  expect(nameHelpers.isValidProjectName('Package-name')).toBe(false);
  expect(nameHelpers.isValidProjectName('Package-Name')).toBe(false);
  expect(nameHelpers.isValidProjectName('@scope/package')).toBe(false);
  expect(nameHelpers.isValidProjectName('@scope/package-name')).toBe(false);
});

test('nameHelpers - cleanNamespace', () => {
  expect(nameHelpers.cleanNamespace('@scope/package')).toBe('Package');
  expect(nameHelpers.cleanNamespace('@scope/package-name')).toBe('PackageName');
  expect(nameHelpers.cleanNamespace('package')).toBe('Package');
  expect(nameHelpers.cleanNamespace('package-name')).toBe('PackageName');
  expect(nameHelpers.cleanNamespace('com.company.app')).toBe('Com.Company.App');
  expect(nameHelpers.cleanNamespace('com.company.app-name')).toBe(
    'Com.Company.AppName',
  );
  expect(nameHelpers.cleanNamespace('com.company.app-name.other')).toBe(
    'Com.Company.AppName.Other',
  );
  expect(nameHelpers.cleanNamespace('com::company::app')).toBe(
    'Com.Company.App',
  );
  expect(nameHelpers.cleanNamespace('com::company::app-name')).toBe(
    'Com.Company.AppName',
  );
  expect(nameHelpers.cleanNamespace('com::company::app-name::other')).toBe(
    'Com.Company.AppName.Other',
  );
});

test('nameHelpers - isValidProjectNamespace', () => {
  expect(nameHelpers.isValidProjectNamespace('package')).toBe(true);
  expect(nameHelpers.isValidProjectNamespace('package-name')).toBe(false);
  expect(nameHelpers.isValidProjectNamespace('Package')).toBe(true);
  expect(nameHelpers.isValidProjectNamespace('Package-name')).toBe(false);
  expect(nameHelpers.isValidProjectNamespace('Package-Name')).toBe(false);
  expect(nameHelpers.isValidProjectNamespace('@scope/package')).toBe(false);
  expect(nameHelpers.isValidProjectNamespace('@scope/package-name')).toBe(
    false,
  );
  expect(nameHelpers.isValidProjectNamespace('com.company.app')).toBe(true);
  expect(nameHelpers.isValidProjectNamespace('com.company.app-name')).toBe(
    false,
  );
  expect(
    nameHelpers.isValidProjectNamespace('com.company.app-name.other'),
  ).toBe(false);
  expect(nameHelpers.isValidProjectNamespace('com::company::app')).toBe(false);
  expect(nameHelpers.isValidProjectNamespace('com::company::app-name')).toBe(
    false,
  );
  expect(
    nameHelpers.isValidProjectNamespace('com::company::app-name::other'),
  ).toBe(false);
});
