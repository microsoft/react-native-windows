/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const fs = require('fs');
const path = require('path');
const glob = require('glob');

const xmldoc = require('xmldoc');

// find VS files matching the pattern
function findFiles(folder, filenamePattern) {
  const files = glob.sync(path.join('**', filenamePattern), {
    cwd: folder,
    ignore: [
      'node_modules/**',
      '**/Debug/**',
      '**/Release/**',
      '**/Generated Files/**',
      '**/packages/**',
    ],
  });

  return files;
}

// finds the windows folder if present
function findWindowsFolder(folder) {
  const winDir = 'windows';
  const joinedDir = path.join(folder, winDir);
  if (fs.existsSync(joinedDir)) {
    return joinedDir;
  }

  return null;
}

// find the visual studio solution file
function findSolutionFile(winFolder, packageJson = {}) {
  if (packageJson.name) {
    const solutionName = packageJson.name;

    // First look for a solution named solutionName.sln
    const slnFile = findFiles(winFolder, solutionName + '.sln')[0];

    if (slnFile) {
      return path.join(winFolder, slnFile);
    }
  }

  // Next look for any solution *.sln
  const allSln = findFiles(winFolder, '*.sln');

  if (allSln.length > 0) {
    return path.join(winFolder, allSln[0]);
  }

  return null;
}

// find the visual studio project file
function findProjectFile(winFolder, packageJson = {}) {
  if (packageJson.name) {
    const projectName = packageJson.name;

    // First look for a project named projectName.vcxproj
    const cppProj = findFiles(winFolder, projectName + '.vcxproj')[0];

    if (cppProj) {
      return path.join(winFolder, cppProj);
    }

    // Next look for a project named projectName.csproj
    const csProj = findFiles(winFolder, projectName + '.csproj')[0];

    if (csProj) {
      return path.join(winFolder, csProj);
    }
  }

  // Next look for any project *.vcxproj
  const allCppProj = findFiles(winFolder, '*.vcxproj');

  if (allCppProj.length > 0) {
    return path.join(winFolder, allCppProj[0]);
  }

  // Next look for any project *.csproj
  const allCsProj = findFiles(winFolder, '*.csproj');

  if (allCsProj.length > 0) {
    return path.join(winFolder, allCsProj[0]);
  }

  return null;
}

function getProjectLanguage(projectPath) {
  if (projectPath.endsWith('.vcxproj')) {
    return 'cpp';
  } else if (projectPath.endsWith('.csproj')) {
    return 'cs';
  }
  return null;
}

// read visual studio project file which is actually a XML doc
function readProjectFile(projectPath) {
  return new xmldoc.XmlDocument(fs.readFileSync(projectPath, 'utf8'));
}

function findProperty(projectXml, propertyName) {
  return projectXml.valueWithPath('PropertyGroup.' + propertyName);
}

function getProjectName(projectXml) {
  return (
    findProperty(projectXml, 'ProjectName') ||
    findProperty(projectXml, 'AssemblyName')
  );
}

function getProjectNamespace(projectXml) {
  return findProperty(projectXml, 'RootNamespace');
}

function getProjectGuid(projectXml) {
  return findProperty(projectXml, 'ProjectGuid');
}

module.exports = {
  findFiles: findFiles,
  findWindowsFolder: findWindowsFolder,
  findSolutionFile: findSolutionFile,
  findProjectFile: findProjectFile,
  getProjectLanguage: getProjectLanguage,
  readProjectFile: readProjectFile,
  getProjectName: getProjectName,
  getProjectNamespace: getProjectNamespace,
  getProjectGuid: getProjectGuid,
};
