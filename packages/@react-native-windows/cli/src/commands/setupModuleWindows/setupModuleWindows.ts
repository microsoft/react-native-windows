/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import chalk from 'chalk';
import {performance} from 'perf_hooks';
import {Ora} from 'ora';
import {execSync} from 'child_process';
import glob from 'glob';

import fs from '@react-native-windows/fs';
import type {Command, Config} from '@react-native-community/cli-types';
import {Telemetry, CodedError} from '@react-native-windows/telemetry';

import {
  newSpinner,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import {initWindowsInternal} from '../initWindows/initWindows';
import {codegenWindowsInternal} from '../codegenWindows/codegenWindows';
import type {SetupModuleWindowsOptions} from './setupModuleWindowsOptions';
import {setupModuleWindowsOptions} from './setupModuleWindowsOptions';

// Placeholder interfaces for future TurboModule spec parsing
// interface Parameter {
//   name: string;
//   type: string;
// }

// interface MethodSignature {
//   name: string;
//   returnType: string;
//   parameters: Parameter[];
// }

export class SetupModuleWindows {
  private actualModuleName?: string;
  private actualProjectPath?: string;
  public root: string;
  public options: SetupModuleWindowsOptions;

  constructor(root: string, options: SetupModuleWindowsOptions) {
    this.root = root;
    this.options = options;
  }

  private async validateEnvironment(): Promise<void> {
    this.verboseMessage('Validating environment...');

    // Check if package.json exists
    const packageJsonPath = path.join(this.root, 'package.json');
    if (!(await fs.exists(packageJsonPath))) {
      throw new CodedError(
        'NoPackageJson',
        'No package.json found. Make sure you are in a React Native project directory.',
      );
    }

    // Check if it's a valid npm package
    try {
      const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));
      if (!pkgJson.name) {
        throw new CodedError(
          'NoProjectName',
          'package.json must have a "name" field.',
        );
      }
      this.verboseMessage(`Project name: ${pkgJson.name}`);
    } catch (error: any) {
      if (error.code === 'NoProjectName') {
        throw error;
      }
      throw new CodedError('NoPackageJson', 'package.json is not valid JSON.');
    }

    // Check if yarn is available
    try {
      execSync('yarn --version', {stdio: 'ignore'});
      this.verboseMessage('Yarn found');
    } catch {
      throw new CodedError(
        'Unknown',
        'Yarn is required but not found. Please install Yarn first.',
      );
    }
  }

  private verboseMessage(message: any) {
    if (this.options.logging) {
      console.log(`[SetupModuleWindows] ${message}`);
    }
  }

  private getModuleName(packageName: string): string {
    // Convert package name to PascalCase module name
    // e.g., "react-native-webview" -> "ReactNativeWebview"
    // e.g., "@react-native-community/slider" -> "ReactNativeCommunitySlider"
    return packageName
      .replace(/[@/-]/g, ' ')
      .split(' ')
      .filter(word => word.length > 0)
      .map(word => word.charAt(0).toUpperCase() + word.slice(1).toLowerCase())
      .join('');
  }

  private async checkForExistingSpec(): Promise<void> {
    this.verboseMessage('Checking for TurboModule spec file...');

    // Look for spec files in common locations, excluding node_modules
    const specPatterns = [
      'Native*.[jt]s',
      'src/**/Native*.[jt]s',
      'lib/**/Native*.[jt]s',
      'js/**/Native*.[jt]s',
      'ts/**/Native*.[jt]s',
    ];

    const specFiles: string[] = [];
    for (const pattern of specPatterns) {
      const matches = glob.sync(pattern, {
        cwd: this.root,
        ignore: ['**/node_modules/**', '**/build/**', '**/dist/**'],
      });
      specFiles.push(...matches);
    }

    // Remove duplicates and filter for actual TurboModule specs
    const uniqueSpecFiles = Array.from(new Set(specFiles));
    const validSpecFiles = await this.filterValidSpecFiles(uniqueSpecFiles);

    if (validSpecFiles.length === 0) {
      throw new CodedError(
        'NoTurboModuleSpec',
        'Create Spec File - TurboModule spec file not found. Please create a TurboModule spec file before running setup-module-windows.',
      );
    } else {
      this.verboseMessage(
        `Found valid spec file(s): ${validSpecFiles.join(', ')}`,
      );
      await this.extractModuleNameFromExistingSpec(validSpecFiles[0]);
    }
  }

  private async filterValidSpecFiles(specFiles: string[]): Promise<string[]> {
    const validFiles: string[] = [];

    for (const file of specFiles) {
      try {
        const filePath = path.join(this.root, file);
        if (await fs.exists(filePath)) {
          const content = await fs.readFile(filePath, 'utf8');

          // Check if it's a valid TurboModule spec file
          if (this.isValidTurboModuleSpec(content)) {
            validFiles.push(file);
          }
        }
      } catch (error) {
        this.verboseMessage(`Could not read spec file ${file}: ${error}`);
      }
    }

    return validFiles;
  }

  private isValidTurboModuleSpec(content: string): boolean {
    // Check for TurboModule indicators
    return (
      content.includes('TurboModule') &&
      (content.includes('export interface Spec') ||
        content.includes('extends TurboModule') ||
        content.includes('TurboModuleRegistry'))
    );
  }

  private async extractModuleNameFromExistingSpec(
    specFilePath: string,
  ): Promise<void> {
    try {
      const fullPath = path.join(this.root, specFilePath);
      const content = await fs.readFile(fullPath, 'utf8');

      // Extract the module name from TurboModuleRegistry.getEnforcing<Spec>('ModuleName')
      const exportMatch = content.match(
        /TurboModuleRegistry\.getEnforcing<Spec>\(['"`]([^'"`]+)['"`]\)/,
      );
      if (exportMatch) {
        this.actualModuleName = exportMatch[1];
        this.verboseMessage(
          `Extracted actual module name: ${this.actualModuleName}`,
        );
      } else {
        this.verboseMessage(
          'Could not extract module name from spec file, using package name conversion',
        );
      }
    } catch (error) {
      this.verboseMessage(`Error reading spec file: ${error}`);
    }
  }

  private getActualModuleName(packageName: string): string {
    // If we extracted the actual module name from an existing spec, use that
    if (this.actualModuleName) {
      return this.actualModuleName;
    }

    // Otherwise, fall back to the package name conversion
    return this.getModuleName(packageName);
  }

  public async getFinalModuleName(): Promise<string> {
    try {
      const packageJsonPath = path.join(this.root, 'package.json');
      const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));
      return this.getActualModuleName(pkgJson.name || 'SampleModule');
    } catch {
      return 'SampleModule';
    }
  }

  public getActualProjectPaths(): {headerPath: string; cppPath: string} {
    if (this.actualProjectPath) {
      return {
        headerPath: `${this.actualProjectPath}.h`,
        cppPath: `${this.actualProjectPath}.cpp`,
      };
    }

    // Fallback to getFinalModuleName for backward compatibility
    const moduleName = this.actualModuleName || 'SampleModule';
    return {
      headerPath: `windows/${moduleName}/${moduleName}.h`,
      cppPath: `windows/${moduleName}/${moduleName}.cpp`,
    };
  }

  private async updatePackageJsonCodegen(): Promise<void> {
    this.verboseMessage(
      'Checking and updating package.json codegen configuration...',
    );

    const packageJsonPath = path.join(this.root, 'package.json');
    const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));

    if (!pkgJson.codegenConfig) {
      const moduleName = this.getActualModuleName(
        pkgJson.name || 'SampleModule',
      );

      pkgJson.codegenConfig = {
        name: moduleName,
        type: 'modules',
        jsSrcsDir: '.',
        windows: {
          namespace: `${moduleName}Specs`,
          outputDirectory: 'codegen',
          generators: ['modulesWindows'],
        },
      };

      await fs.writeFile(packageJsonPath, JSON.stringify(pkgJson, null, 2));
      this.verboseMessage(
        `Added codegenConfig to package.json with module name: ${moduleName}`,
      );
    } else {
      this.verboseMessage('codegenConfig already exists in package.json');
    }
  }

  private async cleanAndInstallDeps(): Promise<void> {
    this.verboseMessage('Installing dependencies...');

    try {
      // Suppress deprecation warnings to clean up output
      const env = {
        ...process.env,
        NODE_NO_WARNINGS: '1',
      };
      execSync('yarn install', {cwd: this.root, stdio: 'inherit', env});
      this.verboseMessage('Dependencies installed');
    } catch (error: any) {
      throw new CodedError(
        'Unknown',
        `Failed to install dependencies: ${error.message}`,
      );
    }
  }

  private async runInitWindows(config: Config): Promise<void> {
    this.verboseMessage(
      `Running init-windows with ${this.options.template} template...`,
    );

    try {
      // Create a modified config with the correct root directory
      const modifiedConfig: Config = {
        ...config,
        root: this.root,
      };

      await initWindowsInternal([], modifiedConfig, {
        template: this.options.template || 'cpp-lib',
        overwrite: true,
        logging: this.options.logging,
        telemetry: this.options.telemetry,
      });
      this.verboseMessage('init-windows completed successfully');
    } catch (error: any) {
      // Check if windows directory was created even with errors
      const windowsDir = path.join(this.root, 'windows');
      if (await fs.exists(windowsDir)) {
        this.verboseMessage(
          'Windows directory exists, continuing despite init-windows warnings',
        );
      } else {
        throw new CodedError(
          'Unknown',
          `Failed to run init-windows: ${error.message}`,
        );
      }
    }
  }

  private async runCodegenWindows(config: Config): Promise<void> {
    this.verboseMessage('Running codegen-windows...');

    try {
      const modifiedConfig: Config = {
        ...config,
        root: this.root,
      };

      await codegenWindowsInternal([], modifiedConfig, {
        logging: this.options.logging,
        telemetry: this.options.telemetry,
      });
      this.verboseMessage('codegen-windows completed successfully');
    } catch (error: any) {
      // Check if codegen directory was created even with errors
      const codegenDir = path.join(this.root, 'codegen');
      if (await fs.exists(codegenDir)) {
        this.verboseMessage(
          'Codegen directory exists, continuing despite codegen-windows warnings',
        );
      } else {
        throw new CodedError(
          'InvalidCodegenConfig',
          `Failed to run codegen-windows: ${error.message}`,
        );
      }
    }
  }

  private async generateStubFiles(): Promise<void> {
    this.verboseMessage('Generating C++ stub files...');

    // Look for codegen directory
    let codegenDir = path.join(this.root, 'codegen');
    if (!(await fs.exists(codegenDir))) {
      this.verboseMessage(
        'No codegen directory found, skipping stub generation',
      );
      return;
    }

    const files = await fs.readdir(codegenDir);
    const specFiles = files.filter(file => file.endsWith('Spec.g.h'));

    if (specFiles.length === 0) {
      this.verboseMessage('No Spec.g.h files found in codegen directory');
      return;
    }

    this.verboseMessage(
      `Found ${specFiles.length} codegen spec file(s): ${specFiles.join(', ')}`,
    );

    const windowsDir = path.join(this.root, 'windows');
    const actualModuleName = await this.getFinalModuleName();
    let moduleDir = path.join(windowsDir, actualModuleName);

    // If the expected directory doesn't exist, find any existing project directory
    if (!(await fs.exists(moduleDir))) {
      try {
        const windowsDirContents = await fs.readdir(windowsDir);
        const projectDirs = [];

        for (const item of windowsDirContents) {
          const itemPath = path.join(windowsDir, item);
          const stats = await fs.stat(itemPath);

          if (
            stats.isDirectory() &&
            !item.startsWith('.') &&
            item !== 'ExperimentalFeatures.props' &&
            !item.endsWith('.sln')
          ) {
            // Check if this directory contains typical project files
            const possibleHeaderFile = path.join(itemPath, `${item}.h`);
            const possibleCppFile = path.join(itemPath, `${item}.cpp`);
            if (
              (await fs.exists(possibleHeaderFile)) ||
              (await fs.exists(possibleCppFile))
            ) {
              projectDirs.push(item);
            }
          }
        }

        if (projectDirs.length > 0) {
          const projectName = projectDirs[0];
          moduleDir = path.join(windowsDir, projectName);
          this.actualProjectPath = path.join(
            'windows',
            projectName,
            projectName,
          );
          this.verboseMessage(
            `Found existing Windows project directory: ${moduleDir}`,
          );
        } else {
          await fs.mkdir(moduleDir, {recursive: true});
          this.actualProjectPath = path.join(
            'windows',
            actualModuleName,
            actualModuleName,
          );
        }
      } catch (error) {
        this.verboseMessage(
          `Error searching for Windows project directory: ${error}`,
        );
        await fs.mkdir(moduleDir, {recursive: true});
        this.actualProjectPath = path.join(
          'windows',
          actualModuleName,
          actualModuleName,
        );
      }
    } else {
      this.actualProjectPath = path.join(
        'windows',
        actualModuleName,
        actualModuleName,
      );
    }

    const projectName = path.basename(moduleDir);

    // Generate header and cpp files
    const headerPath = path.join(moduleDir, `${projectName}.h`);
    const cppPath = path.join(moduleDir, `${projectName}.cpp`);

    const headerContent = await this.generateHeaderStub(projectName);
    await fs.writeFile(headerPath, headerContent);
    this.verboseMessage(`Generated header stub: ${headerPath}`);

    const cppContent = await this.generateCppStub(projectName);
    await fs.writeFile(cppPath, cppContent);
    this.verboseMessage(`Generated cpp stub: ${cppPath}`);
  }

  private async generateHeaderStub(moduleName: string): Promise<string> {
    const namespace = `${moduleName}Specs`;
    const codegenNamespace = `${moduleName}Codegen`;

    return `#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native${moduleName}DataTypes.g.h")
  #include "codegen/Native${moduleName}DataTypes.g.h"
#endif
#include "codegen/Native${moduleName}Spec.g.h"

#include "NativeModules.h"

namespace winrt::${namespace}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE(${moduleName})
struct ${moduleName}
{
  using ModuleSpec = ${codegenNamespace}::${moduleName}Spec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  // Reference function for demonstration (from cpp-lib template)
  // double multiply(double a, double b) noexcept { return a * b; }

  // Hello World example to verify module functionality
  REACT_METHOD(sayHello)
  void sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept;

  // TODO: Add your method implementations here
  // Example:
  // REACT_METHOD(getString)
  // void getString(std::string value, std::function<void(std::string)> const & callback) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::${namespace}`;
  }

  private async generateCppStub(moduleName: string): Promise<string> {
    const namespace = `${moduleName}Specs`;

    return `#include "${moduleName}.h"

namespace winrt::${namespace} {

void ${moduleName}::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void ${moduleName}::sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept {
  std::string result = "Hello " + name + "! Module is working.";
  callback(result);
}

// TODO: Implement your methods here
// Example:
// void ${moduleName}::getString(std::string value, std::function<void(std::string)> const & callback) noexcept {
//   callback("Hello " + value);
// }

} // namespace winrt::${namespace}
`;
  }

  public async run(spinner: Ora, config: Config): Promise<void> {
    await this.validateEnvironment();
    spinner.text = 'Checking for TurboModule spec file...';

    await this.checkForExistingSpec();
    spinner.text = 'Updating package.json...';

    await this.updatePackageJsonCodegen();
    spinner.text = 'Installing dependencies...';

    await this.cleanAndInstallDeps();
    spinner.text = 'Setting up Windows library...';

    await this.runInitWindows(config);
    spinner.text = 'Running Windows codegen...';

    await this.runCodegenWindows(config);

    // Wait a bit for codegen files to be fully written to disk
    this.verboseMessage('Waiting for codegen files to be written...');
    await new Promise<void>(resolve => setTimeout(resolve, 1000));

    spinner.text = 'Generating C++ stub files...';
    await this.generateStubFiles();

    spinner.succeed();
  }
}

/**
 * Sanitizes the given option for telemetry.
 */
function optionSanitizer(
  key: keyof SetupModuleWindowsOptions,
  value: any,
): any {
  switch (key) {
    case 'logging':
    case 'telemetry':
    case 'skipDeps':
    case 'skipBuild':
    case 'template':
      return value === undefined ? false : value;
  }
}

/**
 * Get the extra props to add to the `setup-module-windows` telemetry event.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling `npx @react-native-community/cli setup-module-windows`.
 */
async function setupModuleWindows(
  args: string[],
  config: Config,
  options: SetupModuleWindowsOptions,
) {
  await startTelemetrySession(
    'setup-module-windows',
    config,
    options,
    getDefaultOptions(config, setupModuleWindowsOptions),
    optionSanitizer,
  );

  let setupModuleWindowsError: Error | undefined;
  try {
    await setupModuleWindowsInternal(args, config, options);
  } catch (ex) {
    setupModuleWindowsError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(setupModuleWindowsError);
  }

  await endTelemetrySession(setupModuleWindowsError, getExtraProps);
  setExitProcessWithError(options.logging, setupModuleWindowsError);
}

/**
 * Sets up Windows support for React Native community modules.
 */
export async function setupModuleWindowsInternal(
  args: string[],
  config: Config,
  options: SetupModuleWindowsOptions,
) {
  // Suppress Node.js deprecation warnings to clean up output
  const originalNoWarnings = process.env.NODE_NO_WARNINGS;
  if (!options.logging) {
    process.env.NODE_NO_WARNINGS = '1';
  }

  const startTime = performance.now();
  const spinner = newSpinner(
    'Setting up Windows support for React Native module...',
  );

  try {
    const setup = new SetupModuleWindows(config.root, options);
    await setup.run(spinner, config);
    const endTime = performance.now();

    // Get project paths for display
    const projectPaths = setup.getActualProjectPaths();

    console.log(
      `${chalk.green('Success:')} Windows module setup completed! (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    console.log('');
    console.log(
      chalk.bold('🎉 Your React Native module now supports Windows!'),
    );
    console.log('');
    console.log(chalk.bold('Files created/updated:'));
    console.log(`📄 package.json - Added codegen configuration`);
    console.log(
      `💻 ${projectPaths.headerPath} - C++ header file (implement your methods here)`,
    );
    console.log(
      `⚙️  ${projectPaths.cppPath} - C++ implementation file (add your logic here)`,
    );
    console.log('');
    console.log(chalk.bold('Next steps:'));
    console.log(
      "1. 📝 Update the generated spec file with your module's interface",
    );
    console.log('2. 🔧 Implement the methods in the generated C++ stub files');
    console.log('3. 🏗️  Build your project to verify everything works');
    console.log(
      '4. 📚 See the documentation for more details on TurboModule development',
    );
    console.log('');
    console.log(
      chalk.dim(
        'For help, visit: https://microsoft.github.io/react-native-windows/',
      ),
    );
    console.log('');
  } catch (e) {
    spinner.fail();
    const endTime = performance.now();
    console.log(
      `${chalk.red('Error:')} ${(e as any).toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    throw e;
  } finally {
    // Restore original NODE_NO_WARNINGS setting
    if (originalNoWarnings !== undefined) {
      process.env.NODE_NO_WARNINGS = originalNoWarnings;
    } else {
      delete process.env.NODE_NO_WARNINGS;
    }
  }
}

/**
 * Sets up Windows support for React Native community modules.
 */
export const setupModuleWindowsCommand: Command = {
  name: 'setup-module-windows',
  description:
    'Streamlined setup of Windows support for React Native community modules',
  func: setupModuleWindows,
  options: setupModuleWindowsOptions,
};
