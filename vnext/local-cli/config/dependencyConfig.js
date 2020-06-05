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
  sourceDir: string,    // (opt, req if projects defined) Relative path to the windows implementation under folder, ex: 'windows'
  solutionFile: string, // (opt) Relative path to the module's VS solution file under sourceDir, ex: 'ModuleName.sln'
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

/**
 * Gets the config of any RNW native modules under the target folder.
 * @param {string} folder The absolute path to the target folder.
 * @param {object} userConfig A manually specified override config.
 * @return {object} The config if any RNW native modules exist.
 */
function dependencyConfigWindows(folder, userConfig = {}) {
  const usingManualProjectsOverride = 'projects' in userConfig;

  var projects = usingManualProjectsOverride ? userConfig.projects : [];

  const usingManualNugetPackagesOverride = 'nugetPackages' in userConfig;

  var nugetPackages = usingManualNugetPackagesOverride
    ? userConfig.nugetPackages
    : [];

  var sourceDir = null;
  if (usingManualProjectsOverride && projects.length > 0) {
    // Manaully provided projects, so extract the sourceDir
    if (!('sourceDir' in userConfig) || userConfig.sourceDir === null) {
      throw new Error(
        'sourceDir is required if projects are specified, but it is not specified in react-native.config',
      );
    }
    sourceDir = path.join(folder, userConfig.sourceDir);
  } else if (!usingManualProjectsOverride) {
    // No manually provided projects, try to find sourceDir
    sourceDir = configUtils.findWindowsFolder(folder);
  }

  if (
    sourceDir === null &&
    projects.length === 0 &&
    nugetPackages.length === 0
  ) {
    // Nothing to do here, bail
    return null;
  }

  const usingManualSolutionFile = 'solutionFile' in userConfig;

  var solutionFile = null;
  if (usingManualSolutionFile && userConfig.solutionFile !== null) {
    // Manually provided solutionFile, so extract it
    solutionFile = path.join(sourceDir, userConfig.solutionFile);
  } else if (!usingManualSolutionFile) {
    // No manually provided solutionFile, try to find it
    const solutionFiles = configUtils.findSolutionFiled(sourceDir);
    if (solutionFiles.length === 1) {
      solutionFile = path.join(sourceDir, solutionFile);
    }
  }

  if (usingManualProjectsOverride) {
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
          throw new Error(
            `${item} is required for each project in react-native.config`,
          );
        }
      });

      const projectFile = path.join(sourceDir, projects[i].projectFile);

      const projectContents = configUtils.readProjectFile(projectFile);

      // Calculating (auto) items
      projects[i].projectName = configUtils.getProjectName(projectContents);
      projects[i].projectLang = configUtils.getProjectLanguage(projectFile);
      projects[i].projectGuid = configUtils.getProjectGuid(projectContents);

      if (projects[i].directDependency) {
        // Verifying (req) items
        requiredForDirectDepenencies.forEach(item => {
          if (!(item in projects[i])) {
            throw new Error(
              `${item} is required for each project in react-native.config`,
            );
          }
        });
      }
    }
  } else {
    // No react-native.config, try to heurestically find any projects

    const foundProjects = configUtils.findDependencyProjectFiles(sourceDir);

    for (let i = 0; i < foundProjects.length; i++) {
      const projectFile = path.join(sourceDir, foundProjects[i]);

      const projectLang = configUtils.getProjectLanguage(projectFile);

      const projectContents = configUtils.readProjectFile(projectFile);

      const projectName = configUtils.getProjectName(projectContents);

      const projectGuid = configUtils.getProjectGuid(projectContents);

      const projectNamespace = configUtils.getProjectNamespace(projectContents);

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
  }

  return {
    folder,
    sourceDir: sourceDir.substr(folder.length + 1),
    solutionFile:
      solutionFile !== null ? solutionFile.substr(sourceDir.length + 1) : null,
    projects,
    nugetPackages,
  };
}

module.exports = {
  dependencyConfigWindows: dependencyConfigWindows,
};
