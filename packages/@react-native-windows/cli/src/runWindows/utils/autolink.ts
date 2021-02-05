/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

// Types in this file are inaccurate compared to usage in terms of falsiness.
// We should try to rewrite some of this to do automated schema validation to
// guarantee correct types
/* eslint-disable @typescript-eslint/no-unnecessary-condition */

import * as fs from 'fs';
import * as path from 'path';
import * as chalk from 'chalk';
import {performance} from 'perf_hooks';

import {newSpinner} from './commandWithProgress';
import * as vstools from './vstools';
import * as generatorCommon from '../../generator-common';
import * as configUtils from '../../config/configUtils';

import {Command, Config} from '@react-native-community/cli-types';
import {
  WindowsDependencyConfig,
  ProjectDependency,
} from '../../config/dependencyConfig';
import {Project, WindowsProjectConfig} from '../../config/projectConfig';
import {CodedError} from '@react-native-windows/telemetry';
import {XMLSerializer} from 'xmldom';
const formatter = require('xml-formatter');

/**
 * Locates the react-native-windows directory
 * @param config project configuration
 */
function resolveRnwRoot(projectConfig: WindowsProjectConfig) {
  const rnwPackage = path.dirname(
    require.resolve('react-native-windows/package.json', {
      paths: [projectConfig.folder],
    }),
  );
  return rnwPackage;
}

/**
 * Locates the react-native-windows directory containing template files
 * @param config project configuration
 */
function resolveTemplateRoot(projectConfig: WindowsProjectConfig) {
  const rnwPackage = resolveRnwRoot(projectConfig);
  return path.join(rnwPackage, 'template');
}

/**
 * Logs the given message if verbose is True.
 * @param message The message to log.
 * @param verbose Whether or not verbose logging is enabled.
 */
function verboseMessage(message: any, verbose: boolean) {
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
function updateFile(
  filePath: string,
  expectedContents: string,
  verbose: boolean,
  checkMode: boolean,
): boolean {
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
  } else {
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
// Disabling lint warnings due to high existing cyclomatic complexity
// eslint-disable-next-line complexity
async function updateAutoLink(
  args: string[],
  config: Config,
  options: AutoLinkOptions,
) {
  const startTime = performance.now();
  const verbose = options.logging;
  const checkMode = options.check;
  let changesNecessary = false;

  const spinner = newSpinner(
    checkMode ? 'Checking auto-linked files...' : 'Auto-linking...',
  );

  verboseMessage('', verbose);

  try {
    verboseMessage('Parsing project...', verbose);

    const projectConfig = config.project;

    if (!('windows' in projectConfig) || projectConfig.windows === null) {
      throw new CodedError(
        'NoWindowsConfig',
        'Windows auto-link only supported on Windows app projects',
      );
    }

    const windowsAppConfig: WindowsProjectConfig = projectConfig.windows;
    const rnwRoot = resolveRnwRoot(windowsAppConfig);
    const templateRoot = resolveTemplateRoot(windowsAppConfig);

    if (options.sln) {
      const slnFile = path.join(windowsAppConfig.folder, options.sln);
      windowsAppConfig.solutionFile = path.relative(
        path.join(windowsAppConfig.folder, windowsAppConfig.sourceDir),
        slnFile,
      );
    }

    if (options.proj) {
      const projFile = path.join(windowsAppConfig.folder, options.proj);

      const projectContents = configUtils.readProjectFile(projFile);

      windowsAppConfig.project = {
        projectFile: path.relative(
          path.join(windowsAppConfig.folder, windowsAppConfig.sourceDir),
          projFile,
        ),
        projectName: configUtils.getProjectName(projectContents),
        projectLang: configUtils.getProjectLanguage(projFile),
        projectGuid: configUtils.getProjectGuid(projectContents),
      };
    }

    verboseMessage('Found Windows app project, config:', verbose);
    verboseMessage(windowsAppConfig, verbose);

    const alwaysRequired: Array<keyof WindowsProjectConfig> = [
      'folder',
      'sourceDir',
      'solutionFile',
      'project',
    ];

    alwaysRequired.forEach(item => {
      if (!(item in windowsAppConfig) || windowsAppConfig[item] === null) {
        throw new CodedError(
          'IncompleteConfig',
          `${item} is required but not specified by react-native config`,
          {item: item},
        );
      } else if (
        typeof windowsAppConfig[item] === 'string' &&
        (windowsAppConfig[item] as string).startsWith('Error: ')
      ) {
        throw new CodedError(
          'InvalidConfig',
          `${item} invalid. ${windowsAppConfig[item]}`,
          {item: item},
        );
      }
    });

    const solutionFile = path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      windowsAppConfig.solutionFile,
    );

    const windowsAppProjectConfig = windowsAppConfig.project;

    const projectRequired: Array<keyof Project> = [
      'projectFile',
      'projectName',
      'projectLang',
      'projectGuid',
    ];

    projectRequired.forEach(item => {
      if (
        !(item in windowsAppProjectConfig) ||
        windowsAppProjectConfig[item] === null
      ) {
        throw new CodedError(
          'IncompleteConfig',
          `project.${item} is required but not specified by react-native config`,
          {item: item},
        );
      } else if (
        typeof windowsAppProjectConfig[item] === 'string' &&
        windowsAppProjectConfig[item]!.startsWith('Error: ')
      ) {
        throw new CodedError(
          'InvalidConfig',
          `project.${item} invalid. ${windowsAppProjectConfig[item]}`,
          {item: item},
        );
      }
    });

    const projectFile = path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      windowsAppConfig.project.projectFile,
    );

    const projectDir = path.dirname(projectFile);
    const projectLang = windowsAppConfig.project.projectLang!;

    verboseMessage('Parsing dependencies...', verbose);

    const dependenciesConfig = config.dependencies;

    const windowsDependencies: Record<string, WindowsDependencyConfig> = {};

    for (const dependencyName of Object.keys(dependenciesConfig)) {
      const windowsDependency: WindowsDependencyConfig | undefined =
        dependenciesConfig[dependencyName].platforms.windows;

      if (windowsDependency) {
        verboseMessage(
          `${chalk.bold(dependencyName)} has Windows implementation, config:`,
          verbose,
        );
        verboseMessage(windowsDependency, verbose);

        let dependencyIsValid = true;

        dependencyIsValid = !!(
          dependencyIsValid &&
          'sourceDir' in windowsDependency &&
          windowsDependency.sourceDir &&
          !windowsDependency.sourceDir.startsWith('Error: ')
        );

        if (
          'projects' in windowsDependency &&
          Array.isArray(windowsDependency.projects)
        ) {
          windowsDependency.projects.forEach(project => {
            const itemsToCheck: Array<keyof ProjectDependency> = [
              'projectFile',
              'directDependency',
            ];
            itemsToCheck.forEach(item => {
              dependencyIsValid = !!(
                dependencyIsValid &&
                item in project &&
                project[item] &&
                !project[item]!.toString().startsWith('Error: ')
              );
            });
          });
        }

        if (dependencyIsValid) {
          verboseMessage(`Adding ${chalk.bold(dependencyName)}.`, verbose);
          windowsDependencies[dependencyName] = windowsDependency;
        }
      }
    }

    changesNecessary =
      ensureXAMLDialect(windowsAppConfig, checkMode) || changesNecessary;
    // Generating cs/cpp files for app code consumption
    if (projectLang === 'cs') {
      let csUsingNamespaces = '';
      let csReactPackageProviders = '';

      for (const dependencyName of Object.keys(windowsDependencies)) {
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

      const srcCsFile = path.join(
        templateRoot,
        `${projectLang}-app`,
        'src',
        csFileName,
      );

      const destCsFile = path.join(projectDir, csFileName);

      verboseMessage(
        `Calculating ${chalk.bold(path.basename(destCsFile))}...`,
        verbose,
      );

      const csContents = generatorCommon.resolveContents(srcCsFile, {
        useMustache: true,
        autolinkCsUsingNamespaces: csUsingNamespaces,
        autolinkCsReactPackageProviders: csReactPackageProviders,
      });

      changesNecessary =
        updateFile(destCsFile, csContents, verbose, checkMode) ||
        changesNecessary;
    } else if (projectLang === 'cpp') {
      let cppIncludes = '';
      let cppPackageProviders = '';

      for (const dependencyName of Object.keys(windowsDependencies)) {
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

      const srcCppFile = path.join(
        templateRoot,
        `${projectLang}-app`,
        'src',
        cppFileName,
      );

      const destCppFile = path.join(projectDir, cppFileName);

      verboseMessage(
        `Calculating ${chalk.bold(path.basename(destCppFile))}...`,
        verbose,
      );

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
    const csModuleNames: string[] = [];

    if (projectLang === 'cpp') {
      for (const dependencyName of Object.keys(windowsDependencies)) {
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

    const srcPropsFile = path.join(
      templateRoot,
      `shared-app`,
      'src',
      propsFileName,
    );

    const destPropsFile = path.join(projectDir, propsFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destPropsFile))}...`,
      verbose,
    );

    const propsContents = generatorCommon.resolveContents(srcPropsFile, {
      useMustache: true,
      autolinkPropertiesForProps: propertiesForProps,
    });

    changesNecessary =
      updateFile(destPropsFile, propsContents, verbose, checkMode) ||
      changesNecessary;

    // Generating targets for app project consumption
    let projectReferencesForTargets = '';

    for (const dependencyName of Object.keys(windowsDependencies)) {
      windowsDependencies[dependencyName].projects.forEach(project => {
        if (project.directDependency) {
          const dependencyProjectFile = path.join(
            windowsDependencies[dependencyName].folder,
            windowsDependencies[dependencyName].sourceDir!,
            project.projectFile,
          );

          const relDependencyProjectFile = path.relative(
            projectDir,
            dependencyProjectFile,
          );

          projectReferencesForTargets += `\n    <!-- Projects from ${dependencyName} -->`;
          projectReferencesForTargets += `\n    <ProjectReference Include="$(ProjectDir)${relDependencyProjectFile}">
      <Project>${project.projectGuid}</Project>
    </ProjectReference>`;
        }
      });
    }

    const targetFileName = 'AutolinkedNativeModules.g.targets';

    const srcTargetFile = path.join(
      templateRoot,
      `shared-app`,
      'src',
      targetFileName,
    );

    const destTargetFile = path.join(projectDir, targetFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destTargetFile))}...`,
      verbose,
    );

    const targetContents = generatorCommon.resolveContents(srcTargetFile, {
      useMustache: true,
      autolinkProjectReferencesForTargets: projectReferencesForTargets,
    });

    changesNecessary =
      updateFile(destTargetFile, targetContents, verbose, checkMode) ||
      changesNecessary;

    // Generating project entries for solution
    const projectsForSolution: Project[] = [];

    for (const dependencyName of Object.keys(windowsDependencies)) {
      // Process dependency projects
      windowsDependencies[dependencyName].projects.forEach(project => {
        const dependencyProjectFile = path.join(
          windowsDependencies[dependencyName].folder,
          windowsDependencies[dependencyName].sourceDir!,
          project.projectFile,
        );

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
        projectFile: path.join(
          rnwRoot,
          'Microsoft.ReactNative.Managed/Microsoft.ReactNative.Managed.csproj',
        ),
        projectName: 'Microsoft.ReactNative.Managed',
        projectLang: 'cs',
        projectGuid: '{F2824844-CE15-4242-9420-308923CD76C3}',
      });
      projectsForSolution.push({
        projectFile: path.join(
          rnwRoot,
          'Microsoft.ReactNative.Managed.CodeGen//Microsoft.ReactNative.Managed.CodeGen.csproj',
        ),
        projectName: 'Microsoft.ReactNative.Managed.CodeGen',
        projectLang: 'cs',
        projectGuid: '{ADED4FBE-887D-4271-AF24-F0823BCE7961}',
        projectTypeGuid: vstools.dotNetCoreProjectTypeGuid,
      });
    }

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(solutionFile))} changes...`,
      verbose,
    );

    projectsForSolution.forEach(project => {
      const contentsChanged = vstools.addProjectToSolution(
        solutionFile,
        project,
        verbose,
        checkMode,
      );
      changesNecessary = changesNecessary || contentsChanged;
    });

    spinner.succeed();
    const endTime = performance.now();

    if (!changesNecessary) {
      console.log(
        `${chalk.green(
          'Success:',
        )} No auto-linking changes necessary. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    } else if (checkMode) {
      console.log(
        `${chalk.yellow(
          'Warning:',
        )} Auto-linking changes were necessary but ${chalk.bold(
          '--check',
        )} specified. Run ${chalk.bold(
          "'npx react-native autolink-windows'",
        )} to apply the changes. (${Math.round(endTime - startTime)}ms)`,
      );
      throw new CodedError(
        'NeedAutolinking',
        'Auto-linking changes were necessary but --check was specified',
      );
    } else {
      console.log(
        `${chalk.green(
          'Success:',
        )} Auto-linking changes completed. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    }
  } catch (e) {
    spinner.fail();
    const endTime = performance.now();
    console.log(
      `${chalk.red('Error:')} ${e.toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    throw e;
  }
}

export function ensureXAMLDialect(
  windowsProjectConfig: WindowsProjectConfig,
  checkMode: boolean,
) {
  const slnFile = path.join(
    windowsProjectConfig.folder,
    windowsProjectConfig.sourceDir,
    windowsProjectConfig.solutionFile,
  );
  const buildFlagsProps = path.join(
    path.dirname(slnFile),
    'ExperimentalFeatures.props',
  );
  let changesNeeded = false;
  if (fs.existsSync(buildFlagsProps)) {
    const buildFlagsContents = configUtils.readProjectFile(buildFlagsProps);
    const useWinUI3FromConfig = windowsProjectConfig.useWinUI3;
    const useWinUI3FromBuildFlags =
      configUtils
        .tryFindPropertyValue(buildFlagsContents, 'UseWinUI3')
        ?.toLowerCase() === 'true';
    // use the UseWinUI3 value in react-native.config.js, or if not present, the value from ExperimentalFeatures.props
    changesNeeded = updatePackagesConfigXAMLDialect(
      useWinUI3FromConfig !== undefined
        ? useWinUI3FromConfig
        : useWinUI3FromBuildFlags,
      windowsProjectConfig,
      checkMode,
    );
    if (useWinUI3FromConfig !== undefined) {
      // Make sure BuildFlags matches the value that comes from react-native.config.js
      const node = buildFlagsContents.getElementsByTagName('UseWinUI3');
      const newValue = useWinUI3FromConfig ? 'true' : 'false';
      changesNeeded = changesNeeded || node.item(0)?.textContent !== newValue;
      if (!checkMode && changesNeeded) {
        node.item(0)!.textContent = newValue;
        const buildFlagsOutput = new XMLSerializer().serializeToString(
          buildFlagsContents,
        );
        fs.writeFileSync(buildFlagsProps, buildFlagsOutput);
      }
    }
  }
  return changesNeeded;
}

function updatePackagesConfigXAMLDialect(
  useWinUI3: boolean,
  project: WindowsProjectConfig,
  checkMode: boolean,
) {
  const projectFile = path.join(
    project.folder,
    project.sourceDir,
    project.project!.projectFile,
  );
  const packagesConfig = path.join(
    path.dirname(projectFile),
    'packages.config',
  );

  let changed = false;

  if (fs.existsSync(packagesConfig)) {
    // if we don't have a packages.config, then this is a C# project, in which case we use <PackageReference> and dynamically pick the right XAML package.
    const packagesConfigContents = configUtils.readProjectFile(packagesConfig);
    const packageElements = packagesConfigContents.documentElement.getElementsByTagName(
      'package',
    );
    const winUIPropsPath = path.join(
      resolveRnwRoot(project),
      'PropertySheets/WinUI.props',
    );
    const winuiPropsContents = configUtils.readProjectFile(winUIPropsPath);
    const winui2xVersion = configUtils.tryFindPropertyValue(
      winuiPropsContents,
      'WinUI2xVersion',
    );
    const winui3Version = configUtils.tryFindPropertyValue(
      winuiPropsContents,
      'WinUI3Version',
    );

    const dialects = [
      {id: 'Microsoft.WinUI', version: winui3Version!},
      {id: 'Microsoft.UI.Xaml', version: winui2xVersion!},
    ];
    const keepPkg = useWinUI3 ? dialects[0] : dialects[1];
    const removePkg = useWinUI3 ? dialects[1] : dialects[0];

    const nodesToRemove: Element[] = [];
    let needsToAddKeepPkg = true;
    for (let i = 0; i < packageElements.length; i++) {
      const packageElement = packageElements.item(i)!;
      const idAttr = packageElement!.getAttributeNode('id');
      const id = idAttr!.value;
      if (id === removePkg.id) {
        nodesToRemove.push(packageElement);
        changed = true;
      } else if (id === keepPkg.id) {
        changed =
          changed || packageElement.getAttribute('version') !== keepPkg.version;
        packageElement.setAttribute('version', keepPkg.version!);
        needsToAddKeepPkg = false;
      }
    }

    nodesToRemove.forEach(pkg =>
      packagesConfigContents.documentElement.removeChild(pkg),
    );

    if (needsToAddKeepPkg) {
      const newPkg = packagesConfigContents.createElement('package');

      Object.entries(keepPkg).forEach(([attr, value]) => {
        newPkg.setAttribute(attr, value as string);
      });
      newPkg.setAttribute('targetFramework', 'native');
      packagesConfigContents.documentElement.appendChild(newPkg);
      changed = true;
    }

    if (!checkMode && changed) {
      const serializer = new XMLSerializer();
      const output = serializer.serializeToString(packagesConfigContents);
      const formattedXml = formatter(output, {indentation: '  '});
      fs.writeFileSync(packagesConfig, formattedXml, {encoding: 'utf-8'});
    }
  }
  return changed;
}

function formatXml(input: string) {
  const noNL = input.replace(/[\r\n]/g, '');
  const lines = noNL.split('>');
  let output = '';
  let indent = 0;
  for (const line of lines.map(x => x.trim()).filter(x => x !== '')) {
    if (line.startsWith('</')) {
      indent--;
    }
    output += '  '.repeat(indent) + line.trim() + '>\r\n';
    if (line.endsWith('?')) {
      // header, don't change indent
    } else if (line.endsWith('/')) {
      // self-closing tag: <foo />
    } else if (line.startsWith('<!--')) {
      // xml comment: <!-- foo -->
    } else if (!line.startsWith('</')) {
      indent++;
    }
  }
  if (indent !== 0) throw new Error(`Malformed xml, input was ${input}`);
  return output;
}

interface AutoLinkOptions {
  logging: boolean;
  check: boolean;
  sln?: string;
  proj?: string;
}

export const autoLinkCommand: Command = {
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
      description:
        "Override the app solution file determined by 'react-native config', e.g. windows\\myApp.sln",
      default: undefined,
    },
    {
      name: '--proj [string]',
      description:
        "Override the app project file determined by 'react-native config', e.g. windows\\myApp\\myApp.vcxproj",
      default: undefined,
    },
  ],
};
