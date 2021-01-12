"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.dependencyConfigWindows = void 0;
const path = require("path");
const configUtils = require("./configUtils");
/**
 * Gets the config of any RNW native modules under the target folder.
 * @param userConfig A manually specified override config.
 * @return The config if any RNW native modules exist.
 */
function dependencyConfigWindows(folder, userConfig = {}) {
    if (userConfig === null) {
        return null;
    }
    const usingManualProjectsOverride = 'projects' in userConfig && Array.isArray(userConfig.projects);
    const usingManualNugetPackagesOverride = 'nugetPackages' in userConfig && Array.isArray(userConfig.nugetPackages);
    var result = {
        folder,
        projects: usingManualProjectsOverride ? userConfig.projects : [],
        solutionFile: null,
        nugetPackages: usingManualNugetPackagesOverride
            ? userConfig.nugetPackages
            : [],
    };
    let sourceDir = null;
    if (usingManualProjectsOverride && result.projects.length > 0) {
        // Manually provided projects, so extract the sourceDir
        if (!('sourceDir' in userConfig)) {
            sourceDir =
                'Error: Source dir is required if projects are specified, but it is not specified in react-native.config.';
        }
        else if (userConfig.sourceDir === null) {
            sourceDir =
                'Error: Source dir is required if projects are specified, but it is null in react-native.config.';
        }
        else {
            sourceDir = path.join(folder, userConfig.sourceDir);
        }
    }
    else if (!usingManualProjectsOverride) {
        // No manually provided projects, try to find sourceDir
        if ('sourceDir' in userConfig && userConfig.sourceDir !== null) {
            sourceDir = path.join(folder, userConfig.sourceDir);
        }
        else {
            sourceDir = configUtils.findWindowsFolder(folder);
        }
    }
    if (sourceDir === null) {
        // Try to salvage the missing sourceDir
        if (result.projects.length === 0 && result.nugetPackages.length > 0) {
            // Only nuget packages, no sourceDir required
            return result;
        }
        else if (result.projects.length > 0) {
            // Projects overridden but no sourceDir, assume the sourceDir === folder
            sourceDir = folder;
        }
    }
    else if (sourceDir.startsWith('Error: ')) {
        // Source dir error, bail with error
        result.sourceDir = sourceDir;
        return result;
    }
    if (sourceDir === null) {
        // After everything above, if sourceDir is still null,
        // there's nothing more to look for here, bail
        return null;
    }
    result.sourceDir = path.relative(folder, sourceDir);
    const usingManualSolutionFile = 'solutionFile' in userConfig;
    var solutionFile = null;
    if (usingManualSolutionFile && userConfig.solutionFile !== null) {
        // Manually provided solutionFile, so extract it
        solutionFile = path.join(sourceDir, userConfig.solutionFile);
    }
    else if (!usingManualSolutionFile) {
        // No manually provided solutionFile, try to find it
        const foundSolutions = configUtils.findSolutionFiles(sourceDir);
        if (foundSolutions.length === 1) {
            solutionFile = path.join(sourceDir, foundSolutions[0]);
        }
    }
    result.solutionFile =
        solutionFile !== null ? path.relative(sourceDir, solutionFile) : null;
    if (usingManualProjectsOverride) {
        // react-native.config used, fill out (auto) items for each provided project, verify (req) items are present
        const alwaysRequired = [
            'projectFile',
            'directDependency',
        ];
        for (let project of result.projects) {
            // Verifying (req) items
            var errorFound = false;
            alwaysRequired.forEach(item => {
                if (!(item in project)) {
                    project[item] = `Error: ${item} is required for each project in react-native.config`;
                    errorFound = true;
                }
            });
            if (errorFound) {
                break;
            }
            const projectFile = path.join(sourceDir, project.projectFile);
            const projectContents = configUtils.readProjectFile(projectFile);
            // Calculating (auto) items
            project.projectName = configUtils.getProjectName(projectContents);
            project.projectLang = configUtils.getProjectLanguage(projectFile);
            project.projectGuid = configUtils.getProjectGuid(projectContents);
            if (project.directDependency) {
                // Calculating more (auto) items
                const projectNamespace = configUtils.getProjectNamespace(projectContents);
                if (projectNamespace !== null) {
                    const cppNamespace = projectNamespace.replace(/\./g, '::');
                    const csNamespace = projectNamespace.replace(/::/g, '.');
                    project.cppHeaders = project.cppHeaders || [`winrt/${csNamespace}.h`];
                    project.cppPackageProviders = project.cppPackageProviders || [
                        `${cppNamespace}::ReactPackageProvider`,
                    ];
                    project.csNamespaces = project.csNamespaces || [`${csNamespace}`];
                    project.csPackageProviders = project.csPackageProviders || [
                        `${csNamespace}.ReactPackageProvider`,
                    ];
                }
            }
        }
    }
    else {
        // No react-native.config, try to heurestically find any projects
        const foundProjects = configUtils.findDependencyProjectFiles(sourceDir);
        for (const foundProject of foundProjects) {
            const projectFile = path.join(sourceDir, foundProject);
            const projectLang = configUtils.getProjectLanguage(projectFile);
            const projectContents = configUtils.readProjectFile(projectFile);
            const projectName = configUtils.getProjectName(projectContents);
            const projectGuid = configUtils.getProjectGuid(projectContents);
            const projectNamespace = configUtils.getProjectNamespace(projectContents);
            const directDependency = true;
            let cppHeaders = [];
            let cppPackageProviders = [];
            let csNamespaces = [];
            let csPackageProviders = [];
            if (projectNamespace !== null) {
                const cppNamespace = projectNamespace.replace(/\./g, '::');
                const csNamespace = projectNamespace.replace(/::/g, '.');
                cppHeaders.push(`winrt/${csNamespace}.h`);
                cppPackageProviders.push(`${cppNamespace}::ReactPackageProvider`);
                csNamespaces.push(`${csNamespace}`);
                csPackageProviders.push(`${csNamespace}.ReactPackageProvider`);
            }
            result.projects.push({
                projectFile: path.relative(sourceDir, projectFile),
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
    return result;
}
exports.dependencyConfigWindows = dependencyConfigWindows;
//# sourceMappingURL=dependencyConfig.js.map