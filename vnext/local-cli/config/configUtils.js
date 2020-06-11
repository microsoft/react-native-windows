/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const fs = require('fs');
const path = require('path');
const glob = require('glob');

const xmldom = require('xmldom').DOMParser;
const xpath = require('xpath');

const msbuildSelect = xpath.useNamespaces({
  msbuild: 'http://schemas.microsoft.com/developer/msbuild/2003',
});

/**
 * Search for files matching the pattern under the target folder.
 * @param {string} folder The absolute path to target folder.
 * @param {string} filenamePattern The pattern to search for.
 * @return {array} Return the array of relative file paths.
 */
function findFiles(folder, filenamePattern) {
  const files = glob.sync(path.join('**', filenamePattern), {
    cwd: folder,
    ignore: [
      'node_modules/**',
      '**/Debug/**',
      '**/Release/**',
      '**/WinUI3/**',
      '**/Generated Files/**',
      '**/packages/**',
    ],
  });

  return files;
}

/**
 * Search for the windows sub-folder under the target folder.
 * @param {string} folder The absolute path to the target folder.
 * @return The absolute path to the windows folder, if it exists.
 */
function findWindowsFolder(folder) {
  const winDir = 'windows';
  const joinedDir = path.join(folder, winDir);
  if (fs.existsSync(joinedDir)) {
    return joinedDir;
  }

  return null;
}

/**
 * Checks if the target file path is a RNW solution file by checking if it contains the string "ReactNative".
 * @param {string} filePath The absolute file path to check.
 * @return {boolean} Whether the path is to a RNW solution file.
 */
function isRnwSolution(filePath) {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/ReactNative/) > 0
  );
}

/**
 * Search for the RNW solution files under the target folder.
 * @param {string} winFolder The absolute path to target folder.
 * @return {array} Return the array of relative file paths.
 */
function findSolutionFiles(winFolder) {
  // First search for all potential solution files
  const allSolutions = findFiles(winFolder, '*.sln');

  if (allSolutions.length === 0) {
    // If there're no solution files, return 0
    return [];
  } else if (allSolutions.length === 1) {
    // If there is exactly one solution file, assume it's it
    return [allSolutions[0]];
  }

  var solutionFiles = [];

  // Try to find any solution file that appears to be a react native solution
  for (const solutionFile of allSolutions) {
    if (isRnwSolution(path.join(winFolder, solutionFile))) {
      solutionFiles.push(solutionFile);
    }
  }

  return solutionFiles;
}

/**
 * Checks if the target file path is a RNW lib project file.
 * @param {string} filePath The absolute file path to check.
 * @return {boolean} Whether the path is to a RNW lib project file.
 */
function isRnwDependencyProject(filePath) {
  const projectContents = readProjectFile(filePath);

  const projectLang = getProjectLanguage(filePath);
  if (projectLang === 'cs') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CSharpLib.targets',
    );
  } else if (projectLang === 'cpp') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CppLib.targets',
    );
  }

  return false;
}

/**
 * Search for the RNW lib project files under the target folder.
 * @param {string} winFolder The absolute path to target folder.
 * @return {array} Return the array of relative file paths.
 */
function findDependencyProjectFiles(winFolder) {
  // First, search for all potential project files
  const allCppProj = findFiles(winFolder, '*.vcxproj');
  const allCsProj = findFiles(winFolder, '*.csproj');

  const allProjects = allCppProj.concat(allCsProj);

  if (allProjects.length === 0) {
    // If there're no project files, return 0
    return [];
  }

  var dependencyProjectFiles = [];

  // Try to find any project file that appears to be a dependency project
  for (const projectFile of allProjects) {
    if (isRnwDependencyProject(path.join(winFolder, projectFile))) {
      dependencyProjectFiles.push(projectFile);
    }
  }

  return dependencyProjectFiles;
}

/**
 * Checks if the target file path is a RNW app project file.
 * @param {string} filePath The absolute file path to check.
 * @return {boolean} Whether the path is to a RNW app project file.
 */
function isRnwAppProject(filePath) {
  const projectContents = readProjectFile(filePath);

  const projectLang = getProjectLanguage(filePath);
  if (projectLang === 'cs') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CSharpApp.targets',
    );
  } else if (projectLang === 'cpp') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CppApp.targets',
    );
  }

  return false;
}

/**
 * Search for the RNW app project files under the target folder.
 * @param {string} winFolder The absolute path to target folder.
 * @return {array} Return the array of relative file paths.
 */
function findAppProjectFiles(winFolder) {
  // First, search for all potential project files
  const allCppProj = findFiles(winFolder, '*.vcxproj');
  const allCsProj = findFiles(winFolder, '*.csproj');

  const allProjects = allCppProj.concat(allCsProj);

  if (allProjects.length === 0) {
    // If there're no project files, return 0
    return [];
  }

  var appProjectFiles = [];

  // Try to find any project file that appears to be an app project
  for (const projectFile of allProjects) {
    if (isRnwAppProject(path.join(winFolder, projectFile))) {
      appProjectFiles.push(projectFile);
    }
  }

  return appProjectFiles;
}

/**
 * Returns the programming language of the project file.
 * @param {string} projectPath The project file path to check.
 * @return {string} The language string: cpp, cs, or null if unknown.
 */
function getProjectLanguage(projectPath) {
  if (projectPath.endsWith('.vcxproj')) {
    return 'cpp';
  } else if (projectPath.endsWith('.csproj')) {
    return 'cs';
  }
  return null;
}

/**
 * Reads in the contents of the target project file.
 * @param {string} projectPath The target project file path.
 * @return {object} The project file contents.
 */
function readProjectFile(projectPath) {
  const projectContents = fs.readFileSync(projectPath, 'utf8').toString();
  return new xmldom().parseFromString(projectContents, 'application/xml');
}

/**
 * Search for the given property in the project contents and return its value.
 * @param {object} projectContents The XML project contents.
 * @param {string} propertyName The property to look for.
 * @return {string} The value of the tag if it exists.
 */
function findPropertyValue(projectContents, propertyName) {
  var nodes = msbuildSelect(
    `//msbuild:PropertyGroup/msbuild:${propertyName}`,
    projectContents,
  );

  if (nodes.length > 0) {
    // Take the last one
    return nodes[nodes.length - 1].textContent;
  }

  return null;
}

/**
 * Search for the given import project in the project contents and return if it exists.
 * @param {object} projectContents The XML project contents.
 * @param {string} projectName The project to look for.
 * @return {boolean} If the target exists.
 */
function importProjectExists(projectContents, projectName) {
  var nodes = msbuildSelect(
    `//msbuild:Import[contains(@Project,'${projectName}')]`,
    projectContents,
  );

  return nodes.length > 0;
}

/**
 * Gets the name of the project from the project contents.
 * @param {object} projectContents The XML project contents.
 * @return {string} The project name.
 */
function getProjectName(projectContents) {
  return (
    findPropertyValue(projectContents, 'ProjectName') ||
    findPropertyValue(projectContents, 'AssemblyName') ||
    ''
  );
}

/**
 * Gets the namespace of the project from the project contents.
 * @param {object} projectContents The XML project contents.
 * @return {string} The project namespace.
 */
function getProjectNamespace(projectContents) {
  return findPropertyValue(projectContents, 'RootNamespace');
}

/**
 * Gets the guid of the project from the project contents.
 * @param {object} projectContents The XML project contents.
 * @return {string} The project guid.
 */
function getProjectGuid(projectContents) {
  return findPropertyValue(projectContents, 'ProjectGuid');
}

module.exports = {
  findFiles: findFiles,
  findWindowsFolder: findWindowsFolder,
  isRnwSolution: isRnwSolution,
  findSolutionFiles: findSolutionFiles,
  isRnwDependencyProject: isRnwDependencyProject,
  findDependencyProjectFiles: findDependencyProjectFiles,
  isRnwAppProject: isRnwAppProject,
  findAppProjectFiles: findAppProjectFiles,
  getProjectLanguage: getProjectLanguage,
  readProjectFile: readProjectFile,
  getProjectName: getProjectName,
  getProjectNamespace: getProjectNamespace,
  getProjectGuid: getProjectGuid,
};
