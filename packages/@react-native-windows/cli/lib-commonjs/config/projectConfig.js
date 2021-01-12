"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.projectConfigWindows = void 0;
const path = require("path");
const configUtils = require("./configUtils");
/**
 * Gets the config of any RNW apps under the target folder.
 * @param folder The absolute path to the target folder.
 * @param userConfig A manually specified override config.
 * @return The config if any RNW apps exist.
 */
function projectConfigWindows(folder, userConfig = {}) {
    if (userConfig === null) {
        return null;
    }
    const usingManualOverride = 'sourceDir' in userConfig;
    const sourceDir = usingManualOverride
        ? path.join(folder, userConfig.sourceDir)
        : configUtils.findWindowsFolder(folder);
    if (sourceDir === null) {
        // Nothing to look for here, bail
        return null;
    }
    var result = {
        folder: folder,
        sourceDir: path.relative(folder, sourceDir),
    };
    var validProject = false;
    if (usingManualOverride) {
        // Manual override, try to use it for solutionFile
        if (!('solutionFile' in userConfig)) {
            result.solutionFile =
                'Error: Solution file is required but not specified in react-native.config.';
        }
        else if (userConfig.solutionFile === null) {
            result.solutionFile =
                'Error: Solution file is null in react-native.config.';
        }
        else {
            result.solutionFile = userConfig.solutionFile;
        }
        // Manual override, try to use it for project
        if (!('project' in userConfig)) {
            result.project = {
                projectFile: 'Error: Project is required but not specified in react-native.config.',
            };
        }
        else if (!userConfig.project) {
            result.project = {
                projectFile: 'Error: Project is null in react-native.config.',
            };
        }
        else {
            if (!('projectFile' in userConfig.project)) {
                result.project = {
                    projectFile: 'Error: Project file is required for project in react-native.config.',
                };
            }
            else if (userConfig.project.projectFile === null) {
                result.project = {
                    projectFile: 'Error: Project file is null in react-native.config.',
                };
            }
            else {
                result.project = {
                    projectFile: userConfig.project.projectFile,
                };
                validProject = true;
            }
        }
    }
    else {
        // No manually provided solutionFile, try to find it
        const foundSolutions = configUtils.findSolutionFiles(sourceDir);
        if (foundSolutions.length === 0) {
            result.solutionFile =
                'Error: No app solution file found, please specify in react-native.config.';
        }
        else if (foundSolutions.length > 1) {
            result.solutionFile =
                'Error: Too many app solution files found, please specify in react-native.config.';
        }
        else {
            result.solutionFile = foundSolutions[0];
        }
        // No manually provided project, try to find it
        const foundProjects = configUtils.findAppProjectFiles(sourceDir);
        if (foundProjects.length === 0) {
            result.project = {
                projectFile: 'Error: No app project file found, please specify in react-native.config.',
            };
        }
        else if (foundProjects.length > 1) {
            result.project = {
                projectFile: 'Error: Too many app project files found, please specify in react-native.config.',
            };
        }
        else {
            result.project = {
                projectFile: foundProjects[0],
            };
            validProject = true;
        }
    }
    if (validProject) {
        const projectFile = path.join(sourceDir, result.project.projectFile);
        const projectContents = configUtils.readProjectFile(projectFile);
        // Add missing (auto) items
        result.project.projectName = configUtils.getProjectName(projectContents);
        result.project.projectLang = configUtils.getProjectLanguage(projectFile);
        result.project.projectGuid = configUtils.getProjectGuid(projectContents);
    }
    return result;
}
exports.projectConfigWindows = projectConfigWindows;
//# sourceMappingURL=projectConfig.js.map