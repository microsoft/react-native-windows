"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.autoLinkCommand = void 0;
const fs = require("fs");
const path = require("path");
const chalk = require("chalk");
const perf_hooks_1 = require("perf_hooks");
const commandWithProgress_1 = require("./commandWithProgress");
const vstools = require("./vstools");
const generatorCommon = require("../../generator-common");
const configUtils = require("../../config/configUtils");
/**
 * Locates the react-native-windows directory
 * @param config project configuration
 */
function resolveRnwRoot(projectConfig) {
    const rnwPackage = path.dirname(require.resolve('react-native-windows/package.json', {
        paths: [projectConfig.folder],
    }));
    return rnwPackage;
}
const templateRoot = path.join(__dirname, '../../../templates');
/**
 * Logs the given message if verbose is True.
 * @param message The message to log.
 * @param verbose Whether or not verbose logging is enabled.
 */
function verboseMessage(message, verbose) {
    if (verbose) {
        console.log(message);
    }
}
/**
 * Updates the target file with the expected contents if it's different.
 * @param filePath Path to the target file to update.
 * @param expectedContents The expected contents of the file.
 * @param verbose If true, enable verbose logging.
 * @param checkMode It true, don't make any changes.
 * @return Whether any changes were necessary.
 */
function updateFile(filePath, expectedContents, verbose, checkMode) {
    const fileName = chalk.bold(path.basename(filePath));
    verboseMessage(`Reading ${fileName}...`, verbose);
    const actualContents = fs.existsSync(filePath)
        ? fs.readFileSync(filePath).toString()
        : '';
    const contentsChanged = expectedContents !== actualContents;
    if (contentsChanged) {
        verboseMessage(chalk.yellow(`${fileName} needs to be updated.`), verbose);
        if (!checkMode) {
            verboseMessage(`Writing ${fileName}...`, verbose);
            fs.writeFileSync(filePath, expectedContents, {
                encoding: 'utf8',
                flag: 'w',
            });
        }
    }
    else {
        verboseMessage(`No changes to ${fileName}.`, verbose);
    }
    return contentsChanged;
}
/**
 * Performs auto-linking for RNW native modules and apps.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function updateAutoLink(args, config, options) {
    const startTime = perf_hooks_1.performance.now();
    const verbose = options.logging;
    const checkMode = options.check;
    var changesNecessary = false;
    const spinner = commandWithProgress_1.newSpinner(checkMode ? 'Checking auto-linked files...' : 'Auto-linking...');
    verboseMessage('', verbose);
    try {
        verboseMessage('Parsing project...', verbose);
        const projectConfig = config.project;
        if (!('windows' in projectConfig) || projectConfig.windows === null) {
            throw new Error('Windows auto-link only supported on Windows app projects');
        }
        var windowsAppConfig = projectConfig.windows;
        const rnwRoot = resolveRnwRoot(windowsAppConfig);
        if (options.sln) {
            const slnFile = path.join(windowsAppConfig.folder, options.sln);
            windowsAppConfig.solutionFile = path.relative(path.join(windowsAppConfig.folder, windowsAppConfig.sourceDir), slnFile);
        }
        if (options.proj) {
            const projFile = path.join(windowsAppConfig.folder, options.proj);
            const projectContents = configUtils.readProjectFile(projFile);
            windowsAppConfig.project = {
                projectFile: path.relative(path.join(windowsAppConfig.folder, windowsAppConfig.sourceDir), projFile),
                projectName: configUtils.getProjectName(projectContents),
                projectLang: configUtils.getProjectLanguage(projFile),
                projectGuid: configUtils.getProjectGuid(projectContents),
            };
        }
        verboseMessage('Found Windows app project, config:', verbose);
        verboseMessage(windowsAppConfig, verbose);
        const alwaysRequired = [
            'folder',
            'sourceDir',
            'solutionFile',
            'project',
        ];
        alwaysRequired.forEach(item => {
            if (!(item in windowsAppConfig) || windowsAppConfig[item] === null) {
                throw new Error(`${item} is required but not specified by react-native config`);
            }
            else if (typeof windowsAppConfig[item] === 'string' &&
                windowsAppConfig[item].startsWith('Error: ')) {
                throw new Error(`${item} invalid. ${windowsAppConfig[item]}`);
            }
        });
        const solutionFile = path.join(windowsAppConfig.folder, windowsAppConfig.sourceDir, windowsAppConfig.solutionFile);
        const windowsAppProjectConfig = windowsAppConfig.project;
        const projectRequired = [
            'projectFile',
            'projectName',
            'projectLang',
            'projectGuid',
        ];
        projectRequired.forEach(item => {
            if (!(item in windowsAppProjectConfig) ||
                windowsAppProjectConfig[item] === null) {
                throw new Error(`project.${item} is required but not specified by react-native config`);
            }
            else if (typeof windowsAppProjectConfig[item] === 'string' &&
                windowsAppProjectConfig[item].startsWith('Error: ')) {
                throw new Error(`project.${item} invalid. ${windowsAppProjectConfig[item]}`);
            }
        });
        const projectFile = path.join(windowsAppConfig.folder, windowsAppConfig.sourceDir, windowsAppConfig.project.projectFile);
        const projectDir = path.dirname(projectFile);
        const projectLang = windowsAppConfig.project.projectLang;
        verboseMessage('Parsing dependencies...', verbose);
        const dependenciesConfig = config.dependencies;
        let windowsDependencies = {};
        for (const dependencyName in dependenciesConfig) {
            const windowsDependency = dependenciesConfig[dependencyName].platforms.windows;
            if (windowsDependency) {
                verboseMessage(`${chalk.bold(dependencyName)} has Windows implementation, config:`, verbose);
                verboseMessage(windowsDependency, verbose);
                var dependencyIsValid = true;
                dependencyIsValid = !!(dependencyIsValid &&
                    'sourceDir' in windowsDependency &&
                    windowsDependency.sourceDir &&
                    !windowsDependency.sourceDir.startsWith('Error: '));
                if ('projects' in windowsDependency &&
                    Array.isArray(windowsDependency.projects)) {
                    windowsDependency.projects.forEach(project => {
                        const itemsToCheck = [
                            'projectFile',
                            'directDependency',
                        ];
                        itemsToCheck.forEach(item => {
                            dependencyIsValid = !!(dependencyIsValid &&
                                item in project &&
                                project[item] &&
                                !project[item].toString().startsWith('Error: '));
                        });
                    });
                }
                if (dependencyIsValid) {
                    verboseMessage(`Adding ${chalk.bold(dependencyName)}.`, verbose);
                    windowsDependencies[dependencyName] = windowsDependency;
                }
            }
        }
        // Generating cs/cpp files for app code consumption
        if (projectLang === 'cs') {
            let csUsingNamespaces = '';
            let csReactPackageProviders = '';
            for (const dependencyName in windowsDependencies) {
                windowsDependencies[dependencyName].projects.forEach(project => {
                    if (project.directDependency) {
                        csUsingNamespaces += `\n\n// Namespaces from ${dependencyName}`;
                        project.csNamespaces.forEach(namespace => {
                            csUsingNamespaces += `\nusing ${namespace};`;
                        });
                        csReactPackageProviders += `\n            // IReactPackageProviders from ${dependencyName}`;
                        project.csPackageProviders.forEach(packageProvider => {
                            csReactPackageProviders += `\n            packageProviders.Add(new ${packageProvider}());`;
                        });
                    }
                });
            }
            const csFileName = 'AutolinkedNativeModules.g.cs';
            const srcCsFile = path.join(templateRoot, projectLang, 'src', csFileName);
            const destCsFile = path.join(projectDir, csFileName);
            verboseMessage(`Calculating ${chalk.bold(path.basename(destCsFile))}...`, verbose);
            const csContents = generatorCommon.resolveContents(srcCsFile, {
                useMustache: true,
                autolinkCsUsingNamespaces: csUsingNamespaces,
                autolinkCsReactPackageProviders: csReactPackageProviders,
            });
            changesNecessary =
                updateFile(destCsFile, csContents, verbose, checkMode) ||
                    changesNecessary;
        }
        else if (projectLang === 'cpp') {
            let cppIncludes = '';
            let cppPackageProviders = '';
            for (const dependencyName in windowsDependencies) {
                windowsDependencies[dependencyName].projects.forEach(project => {
                    if (project.directDependency) {
                        cppIncludes += `\n\n// Includes from ${dependencyName}`;
                        project.cppHeaders.forEach(header => {
                            cppIncludes += `\n#include <${header}>`;
                        });
                        cppPackageProviders += `\n    // IReactPackageProviders from ${dependencyName}`;
                        project.cppPackageProviders.forEach(packageProvider => {
                            cppPackageProviders += `\n    packageProviders.Append(winrt::${packageProvider}());`;
                        });
                    }
                });
            }
            if (cppPackageProviders === '') {
                // There are no windows dependencies, this would result in warning. C4100: 'packageProviders': unreferenced formal parameter.
                // therefore add a usage.
                cppPackageProviders = '\n    UNREFERENCED_PARAMETER(packageProviders);'; // CODESYNC: vnext\local-cli\generator-windows\index.js
            }
            const cppFileName = 'AutolinkedNativeModules.g.cpp';
            const srcCppFile = path.join(templateRoot, projectLang, 'src', cppFileName);
            const destCppFile = path.join(projectDir, cppFileName);
            verboseMessage(`Calculating ${chalk.bold(path.basename(destCppFile))}...`, verbose);
            const cppContents = generatorCommon.resolveContents(srcCppFile, {
                useMustache: true,
                autolinkCppIncludes: cppIncludes,
                autolinkCppPackageProviders: cppPackageProviders,
            });
            changesNecessary =
                updateFile(destCppFile, cppContents, verbose, checkMode) ||
                    changesNecessary;
        }
        // Generating props for app project consumption
        let propertiesForProps = '';
        let csModuleNames = [];
        if (projectLang === 'cpp') {
            for (const dependencyName in windowsDependencies) {
                windowsDependencies[dependencyName].projects.forEach(project => {
                    if (project.directDependency && project.projectLang === 'cs') {
                        csModuleNames.push(project.projectName);
                    }
                });
            }
            if (csModuleNames.length > 0) {
                propertiesForProps += `\n    <!-- Set due to dependency on C# module(s): ${csModuleNames.join()} -->`;
                propertiesForProps += `\n    <ConsumeCSharpModules Condition="'$(ConsumeCSharpModules)'==''">true</ConsumeCSharpModules>`;
            }
        }
        const propsFileName = 'AutolinkedNativeModules.g.props';
        const srcPropsFile = path.join(templateRoot, `shared`, 'src', propsFileName);
        const destPropsFile = path.join(projectDir, propsFileName);
        verboseMessage(`Calculating ${chalk.bold(path.basename(destPropsFile))}...`, verbose);
        const propsContents = generatorCommon.resolveContents(srcPropsFile, {
            useMustache: true,
            autolinkPropertiesForProps: propertiesForProps,
        });
        changesNecessary =
            updateFile(destPropsFile, propsContents, verbose, checkMode) ||
                changesNecessary;
        // Generating targets for app project consumption
        let projectReferencesForTargets = '';
        for (const dependencyName in windowsDependencies) {
            windowsDependencies[dependencyName].projects.forEach(project => {
                if (project.directDependency) {
                    const dependencyProjectFile = path.join(windowsDependencies[dependencyName].folder, windowsDependencies[dependencyName].sourceDir, project.projectFile);
                    const relDependencyProjectFile = path.relative(projectDir, dependencyProjectFile);
                    projectReferencesForTargets += `\n    <!-- Projects from ${dependencyName} -->`;
                    projectReferencesForTargets += `\n    <ProjectReference Include="$(ProjectDir)${relDependencyProjectFile}">
      <Project>${project.projectGuid}</Project>
    </ProjectReference>`;
                }
            });
        }
        const targetFileName = 'AutolinkedNativeModules.g.targets';
        const srcTargetFile = path.join(templateRoot, 'shared', 'src', targetFileName);
        const destTargetFile = path.join(projectDir, targetFileName);
        verboseMessage(`Calculating ${chalk.bold(path.basename(destTargetFile))}...`, verbose);
        const targetContents = generatorCommon.resolveContents(srcTargetFile, {
            useMustache: true,
            autolinkProjectReferencesForTargets: projectReferencesForTargets,
        });
        changesNecessary =
            updateFile(destTargetFile, targetContents, verbose, checkMode) ||
                changesNecessary;
        // Generating project entries for solution
        let projectsForSolution = [];
        for (const dependencyName in windowsDependencies) {
            // Process dependency projects
            windowsDependencies[dependencyName].projects.forEach(project => {
                const dependencyProjectFile = path.join(windowsDependencies[dependencyName].folder, windowsDependencies[dependencyName].sourceDir, project.projectFile);
                projectsForSolution.push({
                    projectFile: dependencyProjectFile,
                    projectName: project.projectName,
                    projectLang: project.projectLang,
                    projectGuid: project.projectGuid,
                });
            });
        }
        if (csModuleNames.length > 0) {
            // Add managed projects
            projectsForSolution.push({
                projectFile: path.join(rnwRoot, 'Microsoft.ReactNative.Managed/Microsoft.ReactNative.Managed.csproj'),
                projectName: 'Microsoft.ReactNative.Managed',
                projectLang: 'cs',
                projectGuid: '{F2824844-CE15-4242-9420-308923CD76C3}',
            });
            projectsForSolution.push({
                projectFile: path.join(rnwRoot, 'Microsoft.ReactNative.Managed.CodeGen//Microsoft.ReactNative.Managed.CodeGen.csproj'),
                projectName: 'Microsoft.ReactNative.Managed.CodeGen',
                projectLang: 'cs',
                projectGuid: '{ADED4FBE-887D-4271-AF24-F0823BCE7961}',
                projectTypeGuid: vstools.dotNetCoreProjectTypeGuid,
            });
        }
        verboseMessage(`Calculating ${chalk.bold(path.basename(solutionFile))} changes...`, verbose);
        projectsForSolution.forEach(project => {
            const contentsChanged = vstools.addProjectToSolution(solutionFile, project, verbose, checkMode);
            changesNecessary = changesNecessary || contentsChanged;
        });
        spinner.succeed();
        var endTime = perf_hooks_1.performance.now();
        if (!changesNecessary) {
            console.log(`${chalk.green('Success:')} No auto-linking changes necessary. (${Math.round(endTime - startTime)}ms)`);
        }
        else if (checkMode) {
            console.log(`${chalk.yellow('Warning:')} Auto-linking changes were necessary but ${chalk.bold('--check')} specified. Run ${chalk.bold("'npx react-native autolink-windows'")} to apply the changes. (${Math.round(endTime - startTime)}ms)`);
            throw new Error('Auto-linking changes were necessary but --check was specified');
        }
        else {
            console.log(`${chalk.green('Success:')} Auto-linking changes completed. (${Math.round(endTime - startTime)}ms)`);
        }
    }
    catch (e) {
        spinner.fail();
        var endTime = perf_hooks_1.performance.now();
        console.log(`${chalk.red('Error:')} ${e.toString()}. (${Math.round(endTime - startTime)}ms)`);
        throw e;
    }
}
exports.autoLinkCommand = {
    name: 'autolink-windows',
    description: 'performs autolinking',
    func: updateAutoLink,
    options: [
        {
            name: '--logging',
            description: 'Verbose output logging',
            default: false,
        },
        {
            name: '--check',
            description: 'Only check whether any autolinked files need to change',
            default: false,
        },
        {
            name: '--sln [string]',
            description: "Override the app solution file determined by 'react-native config', e.g. windows\\myApp.sln",
            default: undefined,
        },
        {
            name: '--proj [string]',
            description: "Override the app project file determined by 'react-native config', e.g. windows\\myApp\\myApp.vcxproj",
            default: undefined,
        },
    ],
};
//# sourceMappingURL=autolink.js.map