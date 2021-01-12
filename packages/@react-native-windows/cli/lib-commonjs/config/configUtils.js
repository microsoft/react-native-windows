"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.getProjectGuid = exports.getProjectNamespace = exports.getProjectName = exports.importProjectExists = exports.findPropertyValue = exports.tryFindPropertyValue = exports.readProjectFile = exports.getProjectLanguage = exports.findAppProjectFiles = exports.findDependencyProjectFiles = exports.isRnwDependencyProject = exports.findSolutionFiles = exports.isRnwSolution = exports.findWindowsFolder = exports.findFiles = void 0;
const fs = require("fs");
const path = require("path");
const glob = require("glob");
const xmldom_1 = require("xmldom");
const xpath = require("xpath");
const msbuildSelect = xpath.useNamespaces({
    msbuild: 'http://schemas.microsoft.com/developer/msbuild/2003',
});
/**
 * Search for files matching the pattern under the target folder.
 * @param folder The absolute path to target folder.
 * @param filenamePattern The pattern to search for.
 * @return  Return the array of relative file paths.
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
exports.findFiles = findFiles;
/**
 * Search for the windows sub-folder under the target folder.
 * @param folder The absolute path to the target folder.
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
exports.findWindowsFolder = findWindowsFolder;
/**
 * Checks if the target file path is a RNW solution file by checking if it contains the string "ReactNative".
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW solution file.
 */
function isRnwSolution(filePath) {
    return (fs
        .readFileSync(filePath)
        .toString()
        .search(/ReactNative/) > 0);
}
exports.isRnwSolution = isRnwSolution;
/**
 * Search for the RNW solution files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
 */
function findSolutionFiles(winFolder) {
    // First search for all potential solution files
    const allSolutions = findFiles(winFolder, '*.sln');
    if (allSolutions.length === 0) {
        // If there're no solution files, return 0
        return [];
    }
    else if (allSolutions.length === 1) {
        // If there is exactly one solution file, assume it's it
        return [allSolutions[0]];
    }
    var solutionFiles = [];
    // Try to find any solution file that appears to be a React Native solution
    for (const solutionFile of allSolutions) {
        if (isRnwSolution(path.join(winFolder, solutionFile))) {
            solutionFiles.push(path.normalize(solutionFile));
        }
    }
    return solutionFiles;
}
exports.findSolutionFiles = findSolutionFiles;
/**
 * Checks if the target file path is a RNW lib project file.
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW lib project file.
 */
function isRnwDependencyProject(filePath) {
    const projectContents = readProjectFile(filePath);
    const projectLang = getProjectLanguage(filePath);
    if (projectLang === 'cs') {
        return importProjectExists(projectContents, 'Microsoft.ReactNative.Uwp.CSharpLib.targets');
    }
    else if (projectLang === 'cpp') {
        return importProjectExists(projectContents, 'Microsoft.ReactNative.Uwp.CppLib.targets');
    }
    return false;
}
exports.isRnwDependencyProject = isRnwDependencyProject;
/**
 * Search for the RNW lib project files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
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
            dependencyProjectFiles.push(path.normalize(projectFile));
        }
    }
    return dependencyProjectFiles;
}
exports.findDependencyProjectFiles = findDependencyProjectFiles;
/**
 * Checks if the target file path is a RNW app project file.
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW app project file.
 */
function isRnwAppProject(filePath) {
    const projectContents = readProjectFile(filePath);
    const projectLang = getProjectLanguage(filePath);
    if (projectLang === 'cs') {
        return importProjectExists(projectContents, 'Microsoft.ReactNative.Uwp.CSharpApp.targets');
    }
    else if (projectLang === 'cpp') {
        return importProjectExists(projectContents, 'Microsoft.ReactNative.Uwp.CppApp.targets');
    }
    return false;
}
/**
 * Search for the RNW app project files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
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
            appProjectFiles.push(path.normalize(projectFile));
        }
    }
    return appProjectFiles;
}
exports.findAppProjectFiles = findAppProjectFiles;
/**
 * Returns the programming language of the project file.
 * @param projectPath The project file path to check.
 * @return The language string: cpp, cs, or null if unknown.
 */
function getProjectLanguage(projectPath) {
    if (projectPath.endsWith('.vcxproj')) {
        return 'cpp';
    }
    else if (projectPath.endsWith('.csproj')) {
        return 'cs';
    }
    return null;
}
exports.getProjectLanguage = getProjectLanguage;
/**
 * Reads in the contents of the target project file.
 * @param projectPath The target project file path.
 * @return The project file contents.
 */
function readProjectFile(projectPath) {
    const projectContents = fs.readFileSync(projectPath, 'utf8').toString();
    return new xmldom_1.DOMParser().parseFromString(projectContents, 'application/xml');
}
exports.readProjectFile = readProjectFile;
/**
 * Search for the given property in the project contents and return its value.
 * @param projectContents The XML project contents.
 * @param propertyName The property to look for.
 * @return The value of the tag if it exists.
 */
function tryFindPropertyValue(projectContents, propertyName) {
    var nodes = msbuildSelect(`//msbuild:PropertyGroup/msbuild:${propertyName}`, projectContents);
    if (nodes.length > 0) {
        // Take the last one
        return nodes[nodes.length - 1].textContent;
    }
    return null;
}
exports.tryFindPropertyValue = tryFindPropertyValue;
function findPropertyValue(projectContents, propertyName, filePath) {
    const res = tryFindPropertyValue(projectContents, propertyName);
    if (!res) {
        throw new Error(`Couldn't find property ${propertyName} from ${filePath}`);
    }
    return res;
}
exports.findPropertyValue = findPropertyValue;
/**
 * Search for the given import project in the project contents and return if it exists.
 * @param projectContents The XML project contents.
 * @param projectName The project to look for.
 * @return If the target exists.
 */
function importProjectExists(projectContents, projectName) {
    var nodes = msbuildSelect(`//msbuild:Import[contains(@Project,'${projectName}')]`, projectContents);
    return nodes.length > 0;
}
exports.importProjectExists = importProjectExists;
/**
 * Gets the name of the project from the project contents.
 * @param projectPath The project file path to check.
 * @param projectContents The XML project contents.
 * @return The project name.
 */
function getProjectName(projectContents) {
    const name = tryFindPropertyValue(projectContents, 'ProjectName') ||
        tryFindPropertyValue(projectContents, 'AssemblyName') ||
        '';
    return name;
}
exports.getProjectName = getProjectName;
/**
 * Gets the namespace of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project namespace.
 */
function getProjectNamespace(projectContents) {
    return tryFindPropertyValue(projectContents, 'RootNamespace');
}
exports.getProjectNamespace = getProjectNamespace;
/**
 * Gets the guid of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project guid.
 */
function getProjectGuid(projectContents) {
    return tryFindPropertyValue(projectContents, 'ProjectGuid');
}
exports.getProjectGuid = getProjectGuid;
//# sourceMappingURL=configUtils.js.map