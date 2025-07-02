/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import {commanderNameToOptionName} from '@react-native-windows/telemetry';
import {AutoLinkWindows} from '../commands/autolinkWindows/autolinkWindows';
import {
  AutoLinkOptions,
  autolinkOptions,
} from '../commands/autolinkWindows/autolinkWindowsOptions';
import {ensureWinUI3Project} from './projectConfig.utils';

test('autolink with no windows project', () => {
  expect(() => {
    // eslint-disable-next-line no-new
    new AutoLinkTest({}, {}, {check: true, logging: false});
  }).toThrowError();
});

test('autolink with incomplete windows project', () => {
  expect(() => {
    const autolink = new AutoLinkTest(
      {windows: {}},
      {},
      {check: true, logging: false},
    );
    autolink.validateRequiredAppProperties();
  }).toThrowError();
});

class AutoLinkTest extends AutoLinkWindows {
  public getWindowsProjectConfig() {
    return this.windowsAppConfig;
  }
  protected async updateFile(filepath: string, content: string) {
    return true;
  }
}

test('autolink fixup sln', () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.'}},
    {},
    {check: true, logging: false, sln: 'foo'},
  );
  expect(autolink.getWindowsProjectConfig().solutionFile).toBeUndefined();
  expect(() => {
    autolink.validateRequiredAppProperties();
  }).toThrow();
  autolink.fixUpForSlnOption();
  expect(autolink.getWindowsProjectConfig().solutionFile).toEqual('foo');
  expect(() => {
    autolink.validateRequiredAppProperties();
  }).toThrow();
});

test('autolink fixup proj', async () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.', solutionFile: 'foo.sln'}},
    {},
    {
      check: true,
      logging: false,
      proj: 'projects/WithWinUI3/windows/WithWinUI3/WithWinUI3.vcxproj',
    },
  );
  expect(autolink.getWindowsProjectConfig().solutionFile).toEqual('foo.sln');
  expect(autolink.getWindowsProjectConfig().project).toBeUndefined();

  const folder = path.resolve('src/e2etest/projects/', 'WithWinUI3');
  await ensureWinUI3Project(folder);
  expect(() => {
    autolink.validateRequiredProjectProperties();
  }).toThrow();
  autolink.fixUpForProjOption();

  const projectConfig = autolink.getWindowsProjectConfig().project;
  expect(projectConfig).not.toBeUndefined();
  expect(projectConfig.projectName).toEqual('WithWinUI3');
  autolink.validateRequiredProjectProperties();
});

test('empty cpp autolink dependencies', () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.', solutionFile: 'foo.sln'}},
    {},
    {
      check: true,
      logging: false,
      proj: 'projects/WithWinUI3/windows/WithWinUI3/WithWinUI3.vcxproj',
    },
  );
  const replacements = autolink.getCppReplacements();
  expect(replacements.cppIncludes).toEqual('');
  expect(replacements.cppPackageProviders).toEqual(
    '\n    UNREFERENCED_PARAMETER(packageProviders);',
  );
});

test('one invalid cpp autolink dependency', () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.', solutionFile: 'foo.sln'}},
    {
      superModule: {
        name: 'superModule',
        root: 'theRoot',
        platforms: {
          windows: {},
        },
      },
    },
    {
      check: true,
      logging: false,
      proj: 'projects/WithWinUI3/windows/WithWinUI3/WithWinUI3.vcxproj',
    },
  );
  const replacements = autolink.getCppReplacements();
  expect(replacements.cppIncludes).toEqual('');
  expect(replacements.cppPackageProviders).toEqual(
    '\n    UNREFERENCED_PARAMETER(packageProviders);',
  );
});

test('one invalid cs autolink dependency', () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.', solutionFile: 'foo.sln'}},
    {
      superModule: {
        name: 'superModule',
        root: 'theRoot',
        platforms: {
          windows: {},
        },
      },
    },
    {
      check: true,
      logging: false,
      proj: 'projects/SimpleCSharpApp/windows/SimpleCSharpApp/SimpleCSharpApp.csproj',
    },
  );
  const replacements = autolink.getCsReplacements();
  expect(replacements.csUsingNamespaces).toEqual('');
  expect(replacements.csReactPackageProviders).toEqual('');
});

test('one valid cpp autolink dependency', () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.', solutionFile: 'foo.sln'}},
    {
      superModule: {
        name: 'superModule',
        root: 'theRoot',
        platforms: {
          windows: {
            sourceDir: __dirname,
            projects: [
              {
                directDependency: true,
                projectFile: 'superModule.vcxproj',
                cppHeaders: ['Garfield.h', 'Snoopy.h'],
                cppPackageProviders: ['FamousAnimalCartoons'],
              },
            ],
          },
        },
      },
    },
    {
      check: true,
      logging: false,
      proj: 'projects/WithWinUI3/windows/WithWinUI3/WithWinUI3.vcxproj',
    },
  );
  const replacements = autolink.getCppReplacements();
  expect(replacements.cppIncludes).toMatch(/#include <Garfield.h>/);
  expect(replacements.cppIncludes).toMatch(/#include <Snoopy.h>/);
  expect(replacements.cppPackageProviders).toContain(
    'packageProviders.Append(winrt::FamousAnimalCartoons())',
  );
});

test('one valid cs autolink dependency', () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.', solutionFile: 'foo.sln'}},
    {
      superModule: {
        name: 'superModule',
        root: 'theRoot',
        platforms: {
          windows: {
            sourceDir: __dirname,
            projects: [
              {
                directDependency: true,
                csNamespaces: ['Garfield'],
                projectFile: 'superModule.vcxproj',
                cppHeaders: ['Garfield.h', 'Snoopy.h'],
                csPackageProviders: ['FamousAnimalCartoons'],
              },
            ],
          },
        },
      },
    },
    {
      check: true,
      logging: false,
      proj: 'projects/SimpleCSharpApp/windows/SimpleCSharpApp/SimpleCSharpApp.csproj',
    },
  );
  const replacements = autolink.getCsReplacements();
  expect(replacements.csUsingNamespaces).toContain('using Garfield;');
  expect(replacements.csReactPackageProviders).toContain(
    'packageProviders.Add(new FamousAnimalCartoons())',
  );
});


test('Indirect autolink dependency', () => {
  const autolink = new AutoLinkTest(
    {windows: {folder: __dirname, sourceDir: '.', solutionFile: 'foo.sln'}},
    {
      superModule: {
        name: 'superModule',
        root: 'theRoot',
        platforms: {
          windows: {
            sourceDir: __dirname,
            projects: [
              {
                directDependency: true,
                projectFile: 'superModule.vcxproj',
                cppHeaders: ['Garfield.h', 'Snoopy.h'],
                cppPackageProviders: ['FamousAnimalCartoons'],
              },
              {
                directDependency: false,
                projectFile: 'indirect.vcxproj',
              },
            ],
          },
        },
      },
    },
    {
      check: true,
      logging: false,
      proj: 'projects/WithIndirectDependency/windows/WithIndirectDependency/WithIndirectDependency.vcxproj',
    },
  );
  const replacements = autolink.getCppReplacements();
  expect(replacements.cppIncludes).toMatch(/#include <Garfield.h>/);
  expect(replacements.cppIncludes).toMatch(/#include <Snoopy.h>/);
  expect(replacements.cppPackageProviders).toContain(
    'packageProviders.Append(winrt::FamousAnimalCartoons())',
  );
});

function validateOptionName(
  name: string,
  optionName: keyof AutoLinkOptions,
): boolean {
  // Do not add a default case here. Every item must explicitly return true
  switch (optionName) {
    case 'check':
    case 'logging':
    case 'sln':
    case 'proj':
    case 'telemetry':
      return true;
  }
  throw new Error(
    `Unable to find ${optionName} to match '${name}' in AutoLinkOptions.`,
  );
}

test('autolinkOptions - validate options', () => {
  for (const commandOption of autolinkOptions) {
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

    // Validate all command options are present in AutoLinkOptions
    const optionName = commanderNameToOptionName(commandOption.name);
    expect(
      validateOptionName(
        commandOption.name,
        optionName as keyof AutoLinkOptions,
      ),
    ).toBe(true);
  }
});
