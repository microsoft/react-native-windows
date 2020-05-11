/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const path = require('path');

const configUtils = require('./configUtils.js');

/*

Schema for dependencies:

{
  folder: '', // absolute path to the module root, ex: c:\path\to\node_modules\module-name
  sourceDir: '', // relative path to the windows implementation folder, ex: windows
  solutionFile: '', // relative path to the solution, ex: ModuleName.sln
  projects: [ // array of projects to be built that include modules
    {
      projectFile: '', // relative path to the project, ex: ProjectName\ProjectName.vcxproj
      projectName: '', // name of the project
      projectLang: '', // language of the project, cpp or cs
      projectGuid: '', // project identifier
      cppHeaders: [], // array of cpp header include lines, ie: 'winrt/ModuleName.h', to be transformed into '#include <winrt/ModuleName.h>'
      cppPackageProviders: [], // array of fully qualified cpp IReactPackageProviders, ie: 'ModuleName::ReactPackageProvider'
      csNamespaces: [], // array of cs namespaces, ie: 'ModuleName', to be transformed into 'using ModuleName;'
      csPackageProviders: [], // array of fully qualified cs IReactPackageProviders, ie: 'ModuleName.ReactPackageProvider'
    },
  ],
  additionalProjects: [ // array of (dependency) projects that must be built, but do not contain modules
    {
      projectFile: '', // relative path to the project, ex: ProjectName\ProjectName.vcxproj
      projectName: '', // name of the project
      projectLang: '', // language of the project, cpp or cs
      projectGuid: '', // project identifier
    },
  ],
  packages: [ // array of nuget packages that include modules
    {
      packageName: '', // name of the nuget package to install
      packageVersion: '', // version of the nuget package to install
      cppHeaders: [], // array of cpp header include lines, ie: 'winrt/ModuleName.h', to be transformed into '#include <winrt/ModuleName.h>'
      cppPackageProviders: [], // array of fully qualified cpp IReactPackageProviders, ie: 'ModuleName::ReactPackageProvider'
      csNamespaces: [], // array of cs namespaces, ie: 'ModuleName', to be transformed into 'using ModuleName;'
      csPackageProviders: [], // array of fully qualified cs IReactPackageProviders, ie: 'ModuleName.ReactPackageProvider'
    },
  ],
}

*/

function dependencyConfigWindows(folder, userConfig = {}) {
  if (userConfig.sourceDir) {
    return {
      folder,
      sourceDir: userConfig.sourceDir,
      solutionFile: userConfig.solutionFile,
      projects: userConfig.projects || [],
      additionalProjects: userConfig.additionalProjects || [],
      packages: userConfig.packages || [],
    };
  }

  const sourceDir = configUtils.findWindowsFolder(folder);

  if (!sourceDir) {
    return null;
  }

  const packageJson = require(path.join(folder, 'package.json'));

  const solutionFile = configUtils.findSolutionFile(sourceDir, packageJson);

  const projectFile = configUtils.findProjectFile(sourceDir, packageJson);

  const projectLang = configUtils.getProjectLanguage(projectFile);

  const projectXml = configUtils.readProjectFile(projectFile);

  const projectName = configUtils.getProjectName(projectXml);

  const projectGuid = configUtils.getProjectGuid(projectXml);

  const projectNamespace = configUtils.getProjectNamespace(projectXml);

  const cppNamespace = projectNamespace.replace('.', '::');
  const csNamespace = projectNamespace.replace('::', '.');

  var cppHeaders = [`winrt/${csNamespace}.h`];
  var cppPackageProviders = [`${cppNamespace}::ReactPackageProvider`];
  var csNamespaces = [`${csNamespace}`];
  var csPackageProviders = [`${csNamespace}.ReactPackageProvider`];

  return {
    folder,
    sourceDir: sourceDir.substr(folder.length + 1),
    solutionFile: solutionFile.substr(sourceDir.length + 1),
    projects: [
      {
        projectFile: projectFile.substr(sourceDir.length + 1),
        projectName,
        projectLang,
        projectGuid,
        cppHeaders,
        cppPackageProviders,
        csNamespaces,
        csPackageProviders,
      },
    ],
    additionalProjects: [],
    packages: [],
  };
}

module.exports = {
  dependencyConfigWindows: dependencyConfigWindows,
};
