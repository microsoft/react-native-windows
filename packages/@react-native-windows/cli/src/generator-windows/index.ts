/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import chalk from 'chalk';
import path from 'path';
import username from 'username';
import childProcess from 'child_process';
import crypto from 'crypto';
import fs from '@react-native-windows/fs';
import semver from 'semver';
import _ from 'lodash';
import findUp from 'find-up';
import {
  readProjectFile,
  findPropertyValue,
  tryFindPropertyValueAsBoolean,
} from '../commands/config/configUtils';
import * as nameHelpers from '../utils/nameHelpers';

import {
  createDir,
  copyAndReplaceAll,
  copyAndReplaceWithChangedCallback,
} from '../generator-common';
import {GenerateOptions} from '..';
import {
  CodedError,
  getVersionOfNpmPackage,
} from '@react-native-windows/telemetry';
import {
  findPackage,
  WritableNpmPackage,
} from '@react-native-windows/package-utils';

const windowsDir = 'windows';
const bundleDir = 'Bundle';

/**
 * This represents the data to insert nuget packages
 */
interface NugetPackage {
  id: string;
  version: string;
  privateAssets: boolean;
}

function resolveRnwPath(subpath: string): string {
  return require.resolve(path.join('react-native-windows', subpath), {
    paths: [process.cwd()],
  });
}

// Existing high cyclomatic complexity
// eslint-disable-next-line complexity
export async function copyProjectTemplateAndReplace(
  srcRootPath: string,
  destPath: string,
  newProjectName: string,
  namespace: string,
  options: GenerateOptions,
) {
  if (!srcRootPath) {
    throw new CodedError(
      'CopyProjectTemplateNoSourcePath',
      'Need a path to copy from',
    );
  }

  if (!destPath) {
    throw new CodedError(
      'CopyProjectTemplateNoDestPath',
      'Need a path to copy to',
    );
  }

  if (!newProjectName) {
    throw new CodedError(
      'CopyProjectTemplateNoProjectName',
      'Need a project name',
    );
  }

  const projectType = options.projectType;
  const language = options.language;

  // @react-native-community/cli init only allows alphanumerics in project names, but other
  // new project tools (like expo and create-react-native-module) are less strict.
  // The default (legacy) behavior of this flow is to clean the name rather than throw an error.
  if (!nameHelpers.isValidProjectName(newProjectName)) {
    newProjectName = nameHelpers.cleanName(newProjectName);
  }

  // Similar to the above, but we want to retain namespace separators
  // The default (legacy) behavior of this flow is to clean the name rather than throw an error.
  if (!nameHelpers.isValidProjectNamespace(namespace)) {
    namespace = nameHelpers.cleanNamespace(namespace);
  }

  // Checking if we're overwriting an existing project and re-uses their projectGUID
  const existingProjectPath = path.join(
    destPath,
    windowsDir,
    newProjectName,
    newProjectName + (language === 'cs' ? '.csproj' : '.vcxproj'),
  );
  let existingProjectGuid: string | undefined;
  if (fs.existsSync(existingProjectPath)) {
    console.log('Found existing project, extracting ProjectGuid.');
    existingProjectGuid = findPropertyValue(
      readProjectFile(existingProjectPath),
      'ProjectGuid',
      existingProjectPath,
    ).replace(/[{}]/g, '');
  }

  createDir(path.join(destPath, windowsDir));
  createDir(path.join(destPath, windowsDir, newProjectName));

  if (projectType === 'app') {
    createDir(path.join(destPath, windowsDir, newProjectName, bundleDir));
    createDir(path.join(destPath, windowsDir, newProjectName, 'BundleBuilder'));
  }

  const namespaceCpp = toCppNamespace(namespace);
  if (options.experimentalNuGetDependency) {
    console.log('Using experimental NuGet dependency.');
  }

  const experimentalPropsPath = path.join(
    destPath,
    windowsDir,
    'ExperimentalFeatures.props',
  );

  let existingUseHermes: boolean | null = null;
  if (fs.existsSync(experimentalPropsPath)) {
    existingUseHermes = tryFindPropertyValueAsBoolean(
      readProjectFile(experimentalPropsPath),
      'UseHermes',
    );
  }

  if (existingUseHermes === false) {
    console.warn(
      'Hermes is now the default JS engine and will be enabled for this project. Support for Chakra will be deprecated in the future. To disable Hermes and keep using Chakra for now, see https://microsoft.github.io/react-native-windows/docs/hermes#disabling-hermes.',
    );
  }
  options.useHermes = true;

  if (options.useWinUI3) {
    throw new CodedError(
      'IncompatibleOptions',
      'Experimental WinUI 3 project has been deprecated.',
    );
  }

  const projDir = 'proj';
  const srcPath = path.join(srcRootPath, `${language}-${projectType}`);
  const sharedPath = path.join(srcRootPath, `shared-${projectType}`);
  const projectGuid = existingProjectGuid || crypto.randomUUID();
  const rnwPath = path.dirname(resolveRnwPath('package.json'));
  const rnwVersion = require(resolveRnwPath('package.json')).version;
  const nugetVersion = options.nuGetTestVersion || rnwVersion;
  const packageGuid = crypto.randomUUID();
  const currentUser = username.sync()!; // Gets the current username depending on the platform.

  let mainComponentName = newProjectName;
  const appJsonPath = await findUp('app.json', {cwd: destPath});
  if (appJsonPath) {
    const appJson = await fs.readJsonFile<{name: string}>(appJsonPath);
    mainComponentName = appJson.name;
  }

  // We should prefer putting new, necessary PackageReference dependencies into the appropriate
  // external property sheets, but this is here if we "must" inject the dependency into the project file
  const csNugetPackages: NugetPackage[] = [];
  const cppNugetPackages: NugetPackage[] = [];

  const templateVars: Record<string, any> = {
    useMustache: true,
    regExpPatternsToRemove: [],

    name: newProjectName,
    namespace: namespace,
    namespaceCpp: namespaceCpp,
    languageIsCpp: language === 'cpp',

    rnwVersion: await getVersionOfNpmPackage('react-native-windows'),
    rnwPathFromProjectRoot: path
      .relative(destPath, rnwPath)
      .replace(/\//g, '\\'),

    mainComponentName: mainComponentName,

    // Visual Studio is very picky about the casing of the guids for projects, project references and the solution
    // https://www.bing.com/search?q=visual+studio+project+guid+casing&cvid=311a5ad7f9fc41089507b24600d23ee7&FORM=ANAB01&PC=U531
    // we therefore have to precariously use the right casing in the right place or risk building in VS breaking.
    projectGuidLower: `{${projectGuid.toLowerCase()}}`,
    projectGuidUpper: `{${projectGuid.toUpperCase()}}`,

    // packaging and signing variables:
    packageGuid: packageGuid,
    currentUser: currentUser,

    devMode: options.useDevMode,

    useExperimentalNuget: options.experimentalNuGetDependency,
    nuGetTestFeed: options.nuGetTestFeed,
    nuGetADOFeed: nugetVersion.startsWith('0.0.0-'),

    // cpp template variables
    useWinUI3: options.useWinUI3,
    useHermes: options.useHermes,
    cppNugetPackages: cppNugetPackages,

    // cs template variables
    csNugetPackages: csNugetPackages,

    // autolinking template variables
    autolinkPropertiesForProps: '',
    autolinkProjectReferencesForTargets: '',
    autolinkCsUsingNamespaces: '',
    autolinkCsReactPackageProviders: '',
    autolinkCppIncludes: '',
    autolinkCppPackageProviders:
      '\n    UNREFERENCED_PARAMETER(packageProviders);', // CODESYNC: vnext\local-cli\runWindows\utils\autolink.js
  };

  const commonMappings =
    projectType === 'app'
      ? [
          // app common mappings
          {
            from: path.join(srcRootPath, 'metro.config.js'),
            to: 'metro.config.js',
          },
          {
            from: path.join(srcRootPath, '_gitignore'),
            to: path.join(windowsDir, '.gitignore'),
          },
          {
            from: path.join(srcRootPath, 'b_gitignore'),
            to: path.join(windowsDir, newProjectName, '.gitignore'),
          },
          {
            from: path.join(srcRootPath, 'index.windows.bundle'),
            to: path.join(
              windowsDir,
              newProjectName,
              bundleDir,
              'index.windows.bundle',
            ),
          },
          {
            from: path.join(srcPath, projDir, 'MyApp.sln'),
            to: path.join(windowsDir, newProjectName + '.sln'),
          },
        ]
      : [
          // lib common mappings
          {
            from: path.join(srcRootPath, '_gitignore'),
            to: path.join(windowsDir, '.gitignore'),
          },
          {
            from: path.join(srcPath, projDir, 'MyLib.sln'),
            to: path.join(windowsDir, newProjectName + '.sln'),
          },
        ];

  for (const mapping of commonMappings) {
    await copyAndReplaceWithChangedCallback(
      mapping.from,
      destPath,
      mapping.to,
      templateVars,
      options.overwrite,
    );
  }

  if (language === 'cs') {
    const csMappings =
      projectType === 'app'
        ? [
            // cs app mappings
            {
              from: path.join(srcPath, projDir, 'MyApp.csproj'),
              to: path.join(
                windowsDir,
                newProjectName,
                newProjectName + '.csproj',
              ),
            },
          ]
        : [
            // cs lib mappings
            {
              from: path.join(srcPath, projDir, 'MyLib.csproj'),
              to: path.join(
                windowsDir,
                newProjectName,
                newProjectName + '.csproj',
              ),
            },
          ];

    for (const mapping of csMappings) {
      await copyAndReplaceWithChangedCallback(
        mapping.from,
        destPath,
        mapping.to,
        templateVars,
        options.overwrite,
      );
    }
  } else {
    const cppMappings =
      projectType === 'app'
        ? [
            // cpp app mappings
            {
              from: path.join(srcPath, projDir, 'MyApp.vcxproj'),
              to: path.join(
                windowsDir,
                newProjectName,
                newProjectName + '.vcxproj',
              ),
            },
            {
              from: path.join(srcPath, projDir, 'MyApp.vcxproj.filters'),
              to: path.join(
                windowsDir,
                newProjectName,
                newProjectName + '.vcxproj.filters',
              ),
            },
          ]
        : [
            // cpp lib mappings
            {
              from: path.join(srcPath, projDir, 'MyLib.vcxproj'),
              to: path.join(
                windowsDir,
                newProjectName,
                newProjectName + '.vcxproj',
              ),
            },
            {
              from: path.join(srcPath, projDir, 'MyLib.vcxproj.filters'),
              to: path.join(
                windowsDir,
                newProjectName,
                newProjectName + '.vcxproj.filters',
              ),
            },
            {
              from: path.join(srcPath, projDir, 'MyLib.def'),
              to: path.join(
                windowsDir,
                newProjectName,
                newProjectName + '.def',
              ),
            },
          ];

    for (const mapping of cppMappings) {
      await copyAndReplaceWithChangedCallback(
        mapping.from,
        destPath,
        mapping.to,
        templateVars,
        options.overwrite,
      );
    }
  }

  // shared proj
  if (fs.existsSync(path.join(sharedPath, projDir))) {
    const sharedProjMappings = [];

    sharedProjMappings.push({
      from: path.join(sharedPath, projDir, 'NuGet_Config'),
      to: path.join(windowsDir, 'NuGet.Config'),
    });

    if (
      fs.existsSync(
        path.join(sharedPath, projDir, 'ExperimentalFeatures.props'),
      )
    ) {
      sharedProjMappings.push({
        from: path.join(sharedPath, projDir, 'ExperimentalFeatures.props'),
        to: path.join(windowsDir, 'ExperimentalFeatures.props'),
      });
    }

    for (const mapping of sharedProjMappings) {
      await copyAndReplaceWithChangedCallback(
        mapping.from,
        destPath,
        mapping.to,
        templateVars,
        options.overwrite,
      );
    }
  }

  // shared assets
  if (fs.existsSync(path.join(sharedPath, 'assets'))) {
    await copyAndReplaceAll(
      path.join(sharedPath, 'assets'),
      destPath,
      path.join(windowsDir, newProjectName, 'Assets'),
      templateVars,
      options.overwrite,
    );
  }

  // shared src
  if (fs.existsSync(path.join(sharedPath, 'src'))) {
    await copyAndReplaceAll(
      path.join(sharedPath, 'src'),
      destPath,
      path.join(windowsDir, newProjectName),
      templateVars,
      options.overwrite,
    );
  }

  // src
  if (fs.existsSync(path.join(srcPath, 'src'))) {
    await copyAndReplaceAll(
      path.join(srcPath, 'src'),
      destPath,
      path.join(windowsDir, newProjectName),
      templateVars,
      options.overwrite,
    );
  }

  if (projectType === 'app') {
    console.log(chalk.white.bold('To run your app on UWP:'));
    console.log(chalk.white('   npx @react-native-community/cli run-windows'));
  }
}

function toCppNamespace(namespace: string) {
  return namespace.replace(/\./g, '::');
}

export async function installScriptsAndDependencies(options: {
  verbose: boolean;
}) {
  const projectPackage = await WritableNpmPackage.fromPath(process.cwd());
  if (!projectPackage) {
    throw new Error(
      `The current directory '${process.cwd()}' is not the root of an npm package`,
    );
  }

  await projectPackage.mergeProps({
    scripts: {windows: 'npx @react-native-community/cli run-windows'},
  });

  const rnwPackage = await findPackage('react-native-windows');
  if (!rnwPackage) {
    throw new Error('Could not locate the package for react-native-windows');
  }

  const rnPackage = await findPackage('react-native');
  if (!rnPackage) {
    throw new Error('Could not locate the package for react-native');
  }

  // We add an exclusionList from metro config. This will be hoisted, but add
  // an explicit dep because we require it directly.
  const cliPackage = await findPackage('@react-native-community/cli', {
    searchPath: rnPackage.path,
  });
  const metroConfigPackage = await findPackage('metro-config', {
    searchPath: cliPackage?.path || rnPackage.path,
  });

  if (metroConfigPackage) {
    await projectPackage.mergeProps({
      devDependencies: {
        'metro-config': `^${metroConfigPackage.json.version}`,
      },
    });
  }

  const rnPeerDependency = rnwPackage.json.peerDependencies['react-native'];

  if (
    !semver.satisfies(rnPackage.json.version, rnPeerDependency) &&
    projectPackage.json.dependencies?.['react-native']
  ) {
    console.log(
      chalk.green('Installing a compatible version of react-native:'),
    );
    console.log(chalk.white(`    ${rnPeerDependency}`));

    // Patch package.json to have proper react-native version and install
    await projectPackage.mergeProps({
      dependencies: {'react-native': rnPeerDependency},
    });

    // Install dependencies using correct package manager
    const isYarn = fs.existsSync(path.join(process.cwd(), 'yarn.lock'));
    childProcess.execSync(
      isYarn ? 'yarn' : 'npm i',
      options.verbose ? {stdio: 'inherit'} : {},
    );
  }
}
