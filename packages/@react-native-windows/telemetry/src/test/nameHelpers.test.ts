/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as nameHelpers from '../utils/nameHelpers';

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
  