/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

// Types in this file are inaccurate compared to usage in terms of falsiness.
// We should try to rewrite some of this to do automated schema validation to
// guarantee correct types
/* eslint-disable @typescript-eslint/no-unnecessary-condition */

import fs from '@react-native-windows/fs';
import path from 'path';
import chalk from 'chalk';
import {performance} from 'perf_hooks';

import {Ora} from 'ora';


import type {
  Command,
  Config,
  DependencyConfig,
  ProjectConfig,
} from '@react-native-community/cli-types';
import {Telemetry, CodedError} from '@react-native-windows/telemetry';
import type {AutoLinkOptions} from './autolinkWindowsOptions';
import {autolinkOptions} from './autolinkWindowsOptions';

import {
  newSpinner,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import * as vstools from '../../utils/vstools';
import * as generatorCommon from '../../generator-common';
import * as configUtils from '../config/configUtils';
import * as pathHelpers from '../../utils/pathHelpers';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import type {
  WindowsDependencyConfig,
  ProjectDependency,
} from '../config/dependencyConfig';
import type {Project, WindowsProjectConfig} from '../config/projectConfig';

export class AutoLinkWindows {
  private changesNecessary: boolean;
  protected windowsAppConfig: WindowsProjectConfig;

  public areChangesNeeded() {
    return this.changesNecessary;
  }

  private getWindowsConfig() {
    return this.windowsAppConfig;
  }

  private getSolutionFile() {
    return path.join(
      this.getWindowsConfig().folder,
      this.getWindowsConfig().sourceDir,
      this.getWindowsConfig().solutionFile,
    );
  }

  constructor(
    readonly projectConfig: ProjectConfig,
    readonly dependenciesConfig: {[key: string]: DependencyConfig},
    readonly options: AutoLinkOptions,
  ) {
    this.changesNecessary = false;
    if (
      !('windows' in this.projectConfig) ||
      this.projectConfig.windows === null
    ) {
      throw new CodedError(
        'NoWindowsConfig',
        'Windows auto-link only supported on Windows app projects',
      );
    }
    this.windowsAppConfig = projectConfig.windows;
  }

  public async run(spinner: Ora) {
    const verbose = this.options.logging;

    verboseMessage('', verbose);
    verboseMessage('Parsing project...', verbose);

    const rnwRoot = resolveRnwRoot(this.windowsAppConfig);
    const templateRoot = resolveTemplateRoot(this.windowsAppConfig);

    this.fixUpForSlnOption();
    this.fixUpForProjOption();

    verboseMessage('Found Windows app project, config:', verbose);
    verboseMessage(this.windowsAppConfig, verbose);

    this.validateRequiredAppProperties();

    const solutionFile = this.getSolutionFile();

    const windowsAppProjectConfig = this.windowsAppConfig.project;

    this.validateRequiredProjectProperties();

    const projectFile = this.getProjectFile();

    const projectDir = path.dirname(projectFile);
    const projectLang = windowsAppProjectConfig.projectLang!;

    verboseMessage('Parsing dependencies...', verbose);

    // Generating cs/cpp files for app code consumption
    if (projectLang === 'cs') {
      this.changesNecessary =
        (await this.generateCSAutolinking(templateRoot, projectDir)) ||
        this.changesNecessary;
    } else if (projectLang === 'cpp') {
      this.changesNecessary =
        (await this.generateCppAutolinking(templateRoot, projectDir)) ||
        this.changesNecessary;
    }

    // Generating props for app project consumption
    let propertiesForProps = '';
    let csModuleNames: string[] = [];

    if (projectLang === 'cpp') {
      csModuleNames = this.getCSModules();

      if (csModuleNames.length > 0) {
        propertiesForProps += `
    <!-- Set due to dependency on C# module(s): ${csModuleNames.join()} -->
    <ConsumeCSharpModules Condition="'$(ConsumeCSharpModules)'==''">true</ConsumeCSharpModules>`;
      }
    }

    this.changesNecessary =
      (await this.generateAutolinkProps(
        templateRoot,
        projectDir,
        propertiesForProps,
      )) || this.changesNecessary;

    // Generating targets for app project consumption
    this.changesNecessary =
      (await this.generateAutolinkTargets(projectDir, templateRoot)) ||
      this.changesNecessary;

    // Generating project entries for solution
    this.changesNecessary =
      this.updateSolution(rnwRoot, solutionFile) || this.changesNecessary;

    spinner.succeed();
  }

  /**
   * Handles the --proj command-line option by consuming its value into the windowsAppConfig
   */
  public fixUpForProjOption() {
    if (this.options.proj) {
      const projFile = path.join(
        this.windowsAppConfig.folder,
        this.options.proj,
      );

      const projectContents = configUtils.readProjectFile(projFile);

      this.windowsAppConfig.project = {
        projectFile: path.relative(
          path.join(
            this.windowsAppConfig.folder,
            this.windowsAppConfig.sourceDir,
          ),
          projFile,
        ),
        projectName: configUtils.getProjectName(projFile, projectContents),
        projectLang: configUtils.getProjectLanguage(projFile),
        projectGuid: configUtils.getProjectGuid(projectContents),
      };
    }
  }

  /**
   * Handles the --sln command-line option by consuming its value into the windowsAppConfig
   */
  public fixUpForSlnOption() {
    if (this.options.sln) {
      const slnFile = path.join(this.windowsAppConfig.folder, this.options.sln);
      this.windowsAppConfig.solutionFile = path.relative(
        path.join(
          this.windowsAppConfig.folder,
          this.windowsAppConfig.sourceDir,
        ),
        slnFile,
      );
    }
  }

  /** Validates the all of the required app (solution) properties are present and valid */
  public validateRequiredAppProperties() {
    const alwaysRequired: Array<keyof WindowsProjectConfig> = [
      'folder',
      'sourceDir',
      'solutionFile',
      'project',
    ];

    alwaysRequired.forEach(item => {
      if (
        !(item in this.windowsAppConfig) ||
        this.windowsAppConfig[item] === null
      ) {
        throw new CodedError(
          'IncompleteConfig',
          `${item} is required but not specified by @react-native-community/cli config`,
          {item: item},
        );
      } else if (
        typeof this.windowsAppConfig[item] === 'string' &&
        (this.windowsAppConfig[item] as string).startsWith('Error: ')
      ) {
        throw new CodedError(
          'InvalidConfig',
          `${item} invalid. ${this.windowsAppConfig[item]}`,
          {item: item},
        );
      }
    });
  }

  /** @return the full path to the project file (.vcxproj or .csproj) */
  private getProjectFile() {
    const windowsAppConfig = this.getWindowsConfig();
    return path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      windowsAppConfig.project.projectFile,
    );
  }

  /** Validates that all of the required app _project_ properties are present and valid */
  public validateRequiredProjectProperties() {
    const windowsAppProjectConfig = this.windowsAppConfig.project;
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
          `project.${item} is required but not specified by @react-native-community/cli config`,
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
  }

  private getAutolinkTemplateFile(
    templateRoot: string,
    targetFile: string,
  ): string {
    if (templateRoot.endsWith('\\template')) {
      // Old template split into different folders
      switch (path.extname(targetFile)) {
        case '.cpp':
        case '.h':
          templateRoot = path.join(templateRoot, 'cpp-app', 'src');
          break;
        case '.cs':
          templateRoot = path.join(templateRoot, 'cs-app', 'src');
          break;
        case '.props':
        case '.targets':
        default:
          templateRoot = path.join(templateRoot, 'shared-app', 'src');
          break;
      }
    } else {
      // New template with projected layout
      templateRoot = path.join(templateRoot, 'windows', 'MyApp');
    }

    return path.join(templateRoot, targetFile);
  }

  private async generateCppAutolinking(
    templateRoot: string,
    projectDir: string,
  ) {
    const {cppPackageProviders, cppIncludes} = this.getCppReplacements();

    const cppFileName = 'AutolinkedNativeModules.g.cpp';
    const headerFileName = 'AutolinkedNativeModules.g.h';

    const srcCppFile = this.getAutolinkTemplateFile(templateRoot, cppFileName);
    const srcHeaderFile = this.getAutolinkTemplateFile(
      templateRoot,
      headerFileName,
    );

    const destCppFile = path.join(projectDir, cppFileName);
    const destHeaderFile = path.join(projectDir, headerFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destCppFile))}...`,
      this.options.logging,
    );

    const cppContents = generatorCommon.resolveContents(srcCppFile, {
      useMustache: true,
      autolinkCppIncludes: cppIncludes,
      autolinkCppPackageProviders: cppPackageProviders,
    });

    const cppChanged = await this.updateFile(destCppFile, cppContents);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destHeaderFile))}...`,
      this.options.logging,
    );

    const headerContents = generatorCommon.resolveContents(srcHeaderFile, {
      useMustache: true,
      autolinkCppIncludes: cppIncludes,
      autolinkCppPackageProviders: cppPackageProviders,
    });

    const headerChanged = await this.updateFile(destHeaderFile, headerContents);

    return cppChanged || headerChanged;
  }

  public getCppReplacements() {
    let cppIncludes = '';
    let cppPackageProviders = '';
    const windowsDependencies = this.getWindowsDependencies();

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
      cppPackageProviders = '\n    UNREFERENCED_PARAMETER(packageProviders);'; // CODESYNC: @react-native-windows\cli\src\generator-windows\index.ts
    }
    return {cppPackageProviders, cppIncludes};
  }

  private async generateCSAutolinking(
    templateRoot: string,
    projectDir: string,
  ) {
    const {csUsingNamespaces, csReactPackageProviders} =
      this.getCsReplacements();

    const csFileName = 'AutolinkedNativeModules.g.cs';

    const srcCsFile = this.getAutolinkTemplateFile(templateRoot, csFileName);

    const destCsFile = path.join(projectDir, csFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destCsFile))}...`,
      this.options.logging,
    );

    const csContents = generatorCommon.resolveContents(srcCsFile, {
      useMustache: true,
      autolinkCsUsingNamespaces: csUsingNamespaces,
      autolinkCsReactPackageProviders: csReactPackageProviders,
    });

    return await this.updateFile(destCsFile, csContents);
  }

  public getCsReplacements() {
    let csUsingNamespaces = '';
    let csReactPackageProviders = '';

    const windowsDependencies = this.getWindowsDependencies();
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
    return {csUsingNamespaces, csReactPackageProviders};
  }

  /** Cache of dependencies */
  private windowsDependencies:
    | Record<string, WindowsDependencyConfig>
    | undefined;

  private getWindowsDependencies() {
    if (!this.windowsDependencies) {
      this.windowsDependencies = {};
      for (const dependencyName of Object.keys(this.dependenciesConfig)) {
        const windowsDependency: WindowsDependencyConfig | undefined =
          this.dependenciesConfig[dependencyName].platforms.windows;

        if (windowsDependency) {
          verboseMessage(
            `${chalk.bold(dependencyName)} has Windows implementation, config:`,
            this.options.logging,
          );
          verboseMessage(windowsDependency, this.options.logging);

          let dependencyIsValid = false;

          const hasValidSourceDir =
            'sourceDir' in windowsDependency &&
            windowsDependency.sourceDir !== undefined &&
            !windowsDependency.sourceDir.startsWith('Error: ');

          const hasProjectsInProjectsArray =
            'projects' in windowsDependency &&
            Array.isArray(windowsDependency.projects) &&
            windowsDependency.projects.length > 0;

          if (hasValidSourceDir && hasProjectsInProjectsArray) {
            // Module is source-based and has projects
            dependencyIsValid = true;

            // Validate each source project
            windowsDependency.projects.forEach(project => {
              const itemsToCheck: Array<keyof ProjectDependency> = [
                'projectFile',
                'directDependency',
              ];
              itemsToCheck.forEach(item => {
                dependencyIsValid = !!(
                  dependencyIsValid &&
                  item in project &&
                  project[item] !== '' &&
                  !project[item]!.toString().startsWith('Error: ')
                );
              });
            });
          }

          if (dependencyIsValid) {
            verboseMessage(
              `Adding ${chalk.bold(dependencyName)}.`,
              this.options.logging,
            );
            this.windowsDependencies[dependencyName] = windowsDependency;
          } else {
            verboseMessage(
              `Invalid dependency configuration for dependency ${dependencyName}`,
              this.options.logging,
            );
          }
        }
      }
    }
    return this.windowsDependencies;
  }

  /**
   * Updates the target file with the expected contents if it's different.
   * @param filePath Path to the target file to update.
   * @param expectedContents The expected contents of the file.
   * @return Whether any changes were necessary.
   */
  protected async updateFile(filePath: string, expectedContents: string) {
    const fileName = chalk.bold(path.basename(filePath));
    verboseMessage(`Reading ${fileName}...`, this.options.logging);
    const actualContents = fs.existsSync(filePath)
      ? (await fs.readFile(filePath)).toString()
      : '';

    const contentsChanged = expectedContents !== actualContents;

    if (contentsChanged) {
      verboseMessage(
        chalk.yellow(`${fileName} needs to be updated.`),
        this.options.logging,
      );
      if (!this.options.check) {
        verboseMessage(`Writing ${fileName}...`, this.options.logging);
        await fs.writeFile(filePath, expectedContents, {
          encoding: 'utf8',
          flag: 'w',
        });
      }
    } else {
      verboseMessage(`No changes to ${fileName}.`, this.options.logging);
    }

    return contentsChanged;
  }

  private async generateAutolinkTargets(
    projectDir: string,
    templateRoot: string,
  ) {
    let projectReferencesForTargets = '';

    const windowsDependencies = this.getWindowsDependencies();
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

          projectReferencesForTargets += `
    <!-- Projects from ${dependencyName} -->
    <ProjectReference Include="$(ProjectDir)${relDependencyProjectFile}">
      <Project>${project.projectGuid}</Project>
    </ProjectReference>`;
        }
      });
    }

    const targetFileName = 'AutolinkedNativeModules.g.targets';

    const srcTargetFile = this.getAutolinkTemplateFile(
      templateRoot,
      targetFileName,
    );

    const destTargetFile = path.join(projectDir, targetFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destTargetFile))}...`,
      this.options.logging,
    );

    const targetContents = generatorCommon.resolveContents(srcTargetFile, {
      useMustache: true,
      autolinkProjectReferencesForTargets: projectReferencesForTargets,
    });

    return await this.updateFile(destTargetFile, targetContents);
  }

  private async generateAutolinkProps(
    templateRoot: string,
    projectDir: string,
    propertiesForProps: string,
  ) {
    const propsFileName = 'AutolinkedNativeModules.g.props';

    const srcPropsFile = this.getAutolinkTemplateFile(
      templateRoot,
      propsFileName,
    );

    const destPropsFile = path.join(projectDir, propsFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destPropsFile))}...`,
      this.options.logging,
    );

    const propsContents = generatorCommon.resolveContents(srcPropsFile, {
      useMustache: true,
      autolinkPropertiesForProps: propertiesForProps,
    });

    return await this.updateFile(destPropsFile, propsContents);
  }

  private getCSModules() {
    const csModuleNames: string[] = [];
    const windowsDependencies = this.getWindowsDependencies();
    for (const dependencyName of Object.keys(windowsDependencies)) {
      windowsDependencies[dependencyName].projects.forEach(project => {
        if (project.directDependency && project.projectLang === 'cs') {
          csModuleNames.push(project.projectName);
        }
      });
    }
    return csModuleNames;
  }

  private updateSolution(rnwRoot: string, solutionFile: string) {
    const projectsForSolution: Project[] = [];
    const windowsDependencies = this.getWindowsDependencies();

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

    const csModuleNames = this.getCSModules();
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
      this.options.logging,
    );

    let changesNecessary = false;
    projectsForSolution.forEach(project => {
      const contentsChanged = vstools.addProjectToSolution(
        solutionFile,
        project,
        this.options.logging,
        this.options.check,
      );
      changesNecessary = changesNecessary || contentsChanged;
    });
    return changesNecessary;
  }

  /** @return The CLI command to invoke autolink-windows independently */
  public getAutolinkWindowsCommand() {
    const folder = this.windowsAppConfig.folder;

    const autolinkCommand = 'npx @react-native-community/cli autolink-windows';
    const autolinkArgs = `--sln "${path.relative(
      folder,
      this.getSolutionFile(),
    )}" --proj "${path.relative(folder, this.getProjectFile())}"`;
    return `${autolinkCommand} ${autolinkArgs}`;
  }
}

/**
 * Locates the react-native-windows directory
 * @param config project configuration
 */
function resolveRnwRoot(projectConfig: WindowsProjectConfig) {
  return pathHelpers.resolveRnwRoot(projectConfig.folder);
}

/**
 * Locates the react-native-windows directory containing template files
 * @param config project configuration
 */
function resolveTemplateRoot(projectConfig: WindowsProjectConfig) {
  const rnwPackage = resolveRnwRoot(projectConfig);
  const template = projectConfig.rnwConfig?.['init-windows']?.template as
    | string
    | undefined;

  // For new templates, return the template's root path
  if (template && !template.startsWith('old/')) {
    return path.join(rnwPackage, 'templates', template);
  }

  // For old (unknown templates) fall back to old behavior
  return path.join(rnwPackage, 'template');
}

/**
 * Logs the given message if verbose is True.
 * @param message The message to log.
 * @param verbose Whether or not verbose logging is enabled.
 */
function verboseMessage(message: any, verbose?: boolean) {
  if (verbose) {
    console.log(message);
  }
}

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(key: keyof AutoLinkOptions, value: any): any {
  // Do not add a default case here.
  // Strings risking PII should just return true if present, false otherwise.
  // All others should return the value (or false if undefined).
  switch (key) {
    case 'sln':
    case 'proj':
      return value === undefined ? false : true; // Strip PII
    case 'logging':
    case 'check':
    case 'telemetry':
      return value === undefined ? false : value; // Return value
  }
}

/**
 * Get the extra props to add to the `autolink-windows` telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling `npx @react-native-community/cli autolink-windows`.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function autolinkWindows(
  args: string[],
  config: Config,
  options: AutoLinkOptions,
) {
  await startTelemetrySession(
    'autolink-windows',
    config,
    options,
    getDefaultOptions(config, autolinkOptions),
    optionSanitizer,
  );

  let autolinkWindowsError: Error | undefined;
  try {
    await autolinkWindowsInternal(args, config, options);
  } catch (ex) {
    autolinkWindowsError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(autolinkWindowsError);
  }

  await endTelemetrySession(autolinkWindowsError, getExtraProps);
  setExitProcessWithError(options.logging, autolinkWindowsError);
}

/**
 * Performs auto-linking for RNW native modules and apps.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
export async function autolinkWindowsInternal(
  args: string[],
  config: Config,
  options: AutoLinkOptions,
) {
  const startTime = performance.now();
  const spinner = newSpinner(
    options.check ? 'Checking auto-linked files...' : 'Auto-linking...',
  );
  try {
    const autolink = new AutoLinkWindows(
      config.project,
      config.dependencies,
      options,
    );
    await autolink.run(spinner);
    const endTime = performance.now();

    if (!autolink.areChangesNeeded()) {
      console.log(
        `${chalk.green(
          'Success:',
        )} No auto-linking changes necessary. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    } else if (options.check) {
      const autolinkCommand = autolink.getAutolinkWindowsCommand();
      console.log(
        `${chalk.yellow(
          'Warning:',
        )} Auto-linking changes were necessary but ${chalk.bold(
          '--check',
        )} specified. Run '${chalk.bold(
          `${autolinkCommand}`,
        )}' to apply the changes. (${Math.round(endTime - startTime)}ms)`,
      );
      throw new CodedError(
        'NeedAutolinking',
        `Auto-linking changes were necessary but --check was specified. Run '${autolinkCommand}' to apply the changes`,
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
      `${chalk.red('Error:')} ${(e as any).toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    throw e;
  }
}

/**
 * Performs auto-linking for RNW native modules and apps.
 */
export const autolinkCommand: Command = {
  name: 'autolink-windows',
  description: 'Runs Windows-specific autolinking for your RNW project',
  func: autolinkWindows,
  options: autolinkOptions,
};
