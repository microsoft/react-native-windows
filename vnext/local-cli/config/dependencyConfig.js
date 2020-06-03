/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const path = require('path');

const configUtils = require('./configUtils.js');

/*

react-native config will generate the following JSON for each native module dependency
under node_modules that has a windows implementation, in order to support auto-linking.
This is done heurestically, so if the result isn't quite correct, native module developers
can provide a manual override file: react-native.config.

Schema for dependencies:

Tags:
auto - Item is always calculated by config. An override file should NEVER provide it.
req  - Item is required. If an override file exists, it MUST provide it. If no override file exists, config will try to calculate it.
opt  - Item is optional. If an override file exists, it MAY provide it. If no override file exists, config may try to calculate it.

{
  folder: string,       // (auto) Absolute path to the module root folder, determined by react-native config, ex: 'c:\path\to\app-name\node_modules\module-name'
  sourceDir: string,    // (req) Relative path to the windows implementation under folder, ex: 'windows'
  solutionFile: string, // (req) Relative path to the module's VS solution file under sourceDir, ex: 'ModuleName.sln'
  projects: [ // (opt) Array of VS projects that must be added to the consuming app's solution file, so they are built
    {
      projectFile: string,     // (req) Relative path to the VS project file under sourceDir, ex: 'ProjectName\ProjectName.vcxproj' for 'c:\path\to\app-name\node_modules\module-name\windows\ProjectName\ProjectName.vcxproj'
      projectName: string,     // (auto) Name of the project, determined from projectFile, ex: 'ProjectName'
      projectLang: string,     // (auto) Language of the project, cpp or cs, determined from projectFile
      projectGuid: string,     // (auto) Project identifier, determined from projectFile
      directDependency: bool,  // (req) Whether to add the project file as a dependency to the consuming app's project file. true for projects that provide native modules
      cppHeaders: [],          // (opt, but req if directDependency) Array of cpp header include lines, ie: 'winrt/ModuleName.h', to be transformed into '#include <winrt/ModuleName.h>'
      cppPackageProviders: [], // (opt, but req if directDependency) Array of fully qualified cpp IReactPackageProviders, ie: 'ModuleName::ReactPackageProvider'
      csNamespaces: [],        // (opt, but req if directDependency) Array of cs namespaces, ie: 'ModuleName', to be transformed into 'using ModuleName;'
      csPackageProviders: [],  // (opt, but req if directDependency) Array of fully qualified cs IReactPackageProviders, ie: 'ModuleName.ReactPackageProvider'
    },
  ],
  nugetPackages: [ // (opt) Array of nuget packages including native modules that must be added as a dependency to the consuming app. It can be empty, but by its nature it can't be calculated
    {
      packageName: string,     // (req) Name of the nuget package to install
      packageVersion: string,  // (req) Version of the nuget package to install
      cppHeaders: [],          // (req) Array of cpp header include lines, ie: 'winrt/ModuleName.h', to be transformed into '#include <winrt/ModuleName.h>'
      cppPackageProviders: [], // (req) Array of fully qualified cpp IReactPackageProviders, ie: 'ModuleName::ReactPackageProvider'
      csNamespaces: [],        // (req) Array of cs namespaces, ie: 'ModuleName', to be transformed into 'using ModuleName;'
      csPackageProviders: [],  // (req) Array of fully qualified cs IReactPackageProviders, ie: 'ModuleName.ReactPackageProvider'
    },
  ],
}

*/

function dependencyConfigWindows(folder, userConfig = {}) {
  const usingManualOverride = 'sourceDir' in userConfig;

  const sourceDir = usingManualOverride
    ? path.join(folder, userConfig.sourceDir)
    : configUtils.findWindowsFolder(folder);

  if (sourceDir === null) {
    return null;
  }

  if (usingManualOverride && !('solutionFile' in userConfig)) {
    throw 'solutionFile is required but not specified in react-native.config';
  }

  const packageJson = require(path.join(folder, 'package.json'));

  const solutionFile = usingManualOverride
    ? path.join(sourceDir, userConfig.solutionFile)
    : configUtils.findSolutionFile(sourceDir, packageJson);

  var projects = usingManualOverride ? userConfig.projects || [] : [];
  var nugetPackages = usingManualOverride ? userConfig.nugetPackages || [] : [];

  if (usingManualOverride) {
    // react-native.config used, fill out (auto) items for each provided project, verify (req) items are present

    const alwaysRequired = ['projectFile', 'directDependency'];

    const requiredForDirectDepenencies = [
      'cppHeaders',
      'cppPackageProviders',
      'csNamespaces',
      'csPackageProviders',
    ];

    for (let i = 0; i < projects.length; i++) {
      // Verifying (req) items
      alwaysRequired.forEach(item => {
        if (!(item in projects[i])) {
          throw `${item} is required for each project in react-native.config`;
        }
      });

      const projectFile = path.join(sourceDir, projects[i].projectFile);

      const projectXml = configUtils.readProjectFile(projectFile);

      // Calculating (auto) items

      projects[i].projectName = configUtils.getProjectName(projectXml);
      projects[i].projectLang = configUtils.getProjectLanguage(projectFile);
      projects[i].projectGuid = configUtils.getProjectGuid(projectXml);

      if (projects[i].directDependency) {
        // Verifying (req) items
        requiredForDirectDepenencies.forEach(item => {
          if (!(item in projects[i])) {
            throw `${item} is required for each project in react-native.config`;
          }
        });
      }
    }
  } else {
    // No react-native.config, try to heurestically find the correct project

    const projectFile = configUtils.findProjectFile(sourceDir, packageJson);

    const projectLang = configUtils.getProjectLanguage(projectFile);

    const projectXml = configUtils.readProjectFile(projectFile);

    const projectName = configUtils.getProjectName(projectXml);

    const projectGuid = configUtils.getProjectGuid(projectXml);

    const projectNamespace = configUtils.getProjectNamespace(projectXml);

    const directDependency = true;

    const cppNamespace = projectNamespace.replace(/\./g, '::');
    const csNamespace = projectNamespace.replace(/::/g, '.');

    const cppHeaders = [`winrt/${csNamespace}.h`];
    const cppPackageProviders = [`${cppNamespace}::ReactPackageProvider`];
    const csNamespaces = [`${csNamespace}`];
    const csPackageProviders = [`${csNamespace}.ReactPackageProvider`];

    projects.push({
      projectFile: projectFile.substr(sourceDir.length + 1),
      projectName,
      projectLang,
      projectGuid,
      directDependency,
      cppHeaders,
      cppPackageProviders,
      csNamespaces,
      csPackageProviders,
    });
  }

  return {
    folder,
    sourceDir: sourceDir.substr(folder.length + 1),
    solutionFile: solutionFile.substr(sourceDir.length + 1),
    projects,
    nugetPackages,
  };
}

module.exports = {
  dependencyConfigWindows: dependencyConfigWindows,
};
