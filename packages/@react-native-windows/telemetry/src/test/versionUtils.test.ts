/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as versionUtils from '../utils/versionUtils';

import {lookpath} from 'lookpath';
import path from 'path';
import semver from 'semver';

function expectValidVersion(version: string | null, expectSemVer: boolean) {
  expect(version).not.toBeNull();
  expect(version).toBeDefined();
  if (expectSemVer) {
    expect(semver.valid(version)).toBe(version);
  } else {
    version!.split('.').forEach(s => {
      const tryParseInt = () => {
        parseInt(s, 10);
      };
      expect(tryParseInt).not.toThrow();
    });
  }
}

test('getNodeVersion() is valid', async () => {
  const version = await versionUtils.getNodeVersion();
  expectValidVersion(version, true);
});

test('getNpmVersion() is valid', async () => {
  const node = await lookpath('npm');
  const version = await versionUtils.getNpmVersion();
  if (node) {
    expectValidVersion(version, true);
  } else {
    console.log('npm not installed');
    expect(version).toBeNull();
  }
});

test('getYarnVersion() is valid', async () => {
  const node = await lookpath('yarn');
  const version = await versionUtils.getYarnVersion();
  if (node) {
    expectValidVersion(version, true);
  } else {
    console.log('yarn not installed');
    expect(version).toBeNull();
  }
});

test('getVisualStudioVersion() is valid', async () => {
  const version = await versionUtils.getVisualStudioVersion();
  expectValidVersion(version, false);
});

test('getVersionOfNpmPackage() of empty string is null', async () => {
  const version = await versionUtils.getVersionOfNpmPackage('');
  expect(version).toBeNull();
});

test('getVersionOfNpmPackage() of invalid package is null', async () => {
  const version = await versionUtils.getVersionOfNpmPackage('invalidpackage');
  expect(version).toBeNull();
});

test('getVersionOfNpmPackage() of valid package is valid', async () => {
  const version = await versionUtils.getVersionOfNpmPackage('jest');
  expectValidVersion(version, true);
});

test('getVersionsOfNuGetPackages() of valid package in packages.config is valid', async () => {
  const projectFile = path.resolve(
    __dirname,
    'projects/UsesPackagesConfig/UsesPackagesConfig.vcxproj',
  );
  const versions = await versionUtils.getVersionsOfNuGetPackages(projectFile, [
    'Microsoft.Windows.CppWinRT',
  ]);
  expect(versions).not.toBeNull();
  expectValidVersion(versions['Microsoft.Windows.CppWinRT'], false);
});

test('getVersionsOfNuGetPackages() of invalid package in packages.config is invalid', async () => {
  const projectFile = path.resolve(
    __dirname,
    'projects/UsesPackagesConfig/UsesPackagesConfig.vcxproj',
  );
  const versions = await versionUtils.getVersionsOfNuGetPackages(projectFile, [
    'Invalid.Package',
  ]);
  expect(versions).toStrictEqual({});
});

test('getVersionsOfNuGetPackages() of valid package in project is valid', async () => {
  const projectFile = path.resolve(
    __dirname,
    'projects/UsesPackageReference/UsesPackageReference.csproj',
  );
  const versions = await versionUtils.getVersionsOfNuGetPackages(projectFile, [
    'Microsoft.NETCore.UniversalWindowsPlatform',
  ]);
  expect(versions).not.toBeNull();
  expectValidVersion(
    versions['Microsoft.NETCore.UniversalWindowsPlatform'],
    false,
  );
});

test('getVersionsOfNuGetPackages() of invalid package in project is invalid', async () => {
  const projectFile = path.resolve(
    __dirname,
    'projects/UsesPackageReference/UsesPackageReference.csproj',
  );
  const versions = await versionUtils.getVersionsOfNuGetPackages(projectFile, [
    'Invalid.Package',
  ]);
  expect(versions).toStrictEqual({});
});
