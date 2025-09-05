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

interface Parameter {
  name: string;
  type: string;
}

interface MethodSignature {
  name: string;
  returnType: string;
  parameters: Parameter[];
}

export class SetupModuleWindows {
  private actualModuleName?: string;
  private actualProjectPath?: string;
  public root: string;
  public options: SetupModuleWindowsOptions;

  constructor(root: string, options: SetupModuleWindowsOptions) {
    this.root = root;
    this.options = options;
  }

  private verboseMessage(message: any) {
    if (this.options.logging) {
      console.log(`[SetupModuleWindows] ${message}`);
    }
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

  // Step 1: Check for TurboModule spec, create one with Hello World if not found
  private async checkAndCreateSpecFile(): Promise<void> {
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
      this.verboseMessage(
        'No TurboModule spec file found, creating Hello World spec...',
      );
      await this.createHelloWorldSpecFile();
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

  private getActualModuleName(packageName: string): string {
    // If we extracted the actual module name from an existing spec, use that
    if (this.actualModuleName) {
      return this.actualModuleName;
    }

    // Otherwise, fall back to the package name conversion
    return this.getModuleName(packageName);
  }

  private async createHelloWorldSpecFile(): Promise<void> {
    const pkgJson = JSON.parse(
      await fs.readFile(path.join(this.root, 'package.json'), 'utf8'),
    );
    const moduleName = this.getActualModuleName(pkgJson.name || 'SampleModule');

    const specContent = this.generateHelloWorldSpecFileContent(moduleName);
    const specPath = path.join(this.root, `Native${moduleName}.ts`);
    await fs.writeFile(specPath, specContent);
    this.verboseMessage(`Created Hello World spec file: ${specPath}`);
  }

  private generateHelloWorldSpecFileContent(moduleName: string): string {
    return `/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  // Hello World method - A simple greeting function to verify the module works
  sayHello(name: string): Promise<string>;
  
  // Reference multiply function - Keep this as a learning example
  // This demonstrates how to handle numeric operations in a TurboModule
  multiply(a: number, b: number): Promise<number>;
  
  // Example async method with no parameters
  getRandomNumber(): Promise<number>;
  
  // Example method with boolean return
  isModuleReady(): Promise<boolean>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('${moduleName}');
`;
  }

  // Step 2: Update package.json with codegen configuration
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
          outputDirectory: `windows/${moduleName}/codegen`,
          separateDataTypes: true,
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

  // Step 3 & 4: Clean node_modules and install dependencies
  private async cleanAndInstallDeps(): Promise<void> {
    this.verboseMessage('Cleaning node_modules and installing dependencies...');

    const nodeModulesPath = path.join(this.root, 'node_modules');
    if (await fs.exists(nodeModulesPath)) {
      this.verboseMessage('Removing node_modules directory...');
      await this.removeDirectoryRecursively(nodeModulesPath);
      this.verboseMessage('Removed node_modules');
    }

    try {
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

  private async removeDirectoryRecursively(dirPath: string): Promise<void> {
    try {
      const stats = await fs.stat(dirPath);
      if (stats.isDirectory()) {
        const files = await fs.readdir(dirPath);
        await Promise.all(
          files.map(async file => {
            const filePath = path.join(dirPath, file);
            await this.removeDirectoryRecursively(filePath);
          }),
        );
        await fs.rmdir(dirPath);
      } else {
        await fs.unlink(dirPath);
      }
    } catch (error: any) {
      // Ignore errors if file/directory doesn't exist
      if (error.code !== 'ENOENT') {
        throw error;
      }
    }
  }

  // Step 5 & 6: Upgrade dependencies and install again
  private async upgradeDependencies(): Promise<void> {
    if (this.options.skipDeps) {
      this.verboseMessage('Skipping dependency upgrades');
      return;
    }

    this.verboseMessage(
      'Upgrading React Native and React Native Windows to latest versions...',
    );

    try {
      const env = {
        ...process.env,
        NODE_NO_WARNINGS: '1',
      };
      // Get latest versions with timeout to avoid hanging
      const rnLatest = execSync('npm view react-native version', {
        encoding: 'utf8',
        timeout: 30000,
        env,
      }).trim();
      const rnwLatest = execSync('npm view react-native-windows version', {
        encoding: 'utf8',
        timeout: 30000,
        env,
      }).trim();

      this.verboseMessage(`Latest RN version: ${rnLatest}`);
      this.verboseMessage(`Latest RNW version: ${rnwLatest}`);

      // Update package.json
      const packageJsonPath = path.join(this.root, 'package.json');
      const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));

      if (!pkgJson.peerDependencies) {
        pkgJson.peerDependencies = {};
      }
      if (!pkgJson.devDependencies) {
        pkgJson.devDependencies = {};
      }

      pkgJson.peerDependencies['react-native'] = `^${rnLatest}`;
      pkgJson.devDependencies['react-native-windows'] = `^${rnwLatest}`;

      await fs.writeFile(packageJsonPath, JSON.stringify(pkgJson, null, 2));
      this.verboseMessage('Updated dependency versions in package.json');

      // Install updated dependencies with timeout
      execSync('yarn install', {
        cwd: this.root,
        stdio: 'inherit',
        timeout: 120000,
        env,
      });
    } catch (error: any) {
      this.verboseMessage(
        `Warning: Could not upgrade dependencies: ${error.message}`,
      );
      // Don't fail the entire process if dependency upgrade fails
    }
  }

  // Step 7: Run init-windows with cpp-lib template
  private async runInitWindows(config: Config): Promise<void> {
    this.verboseMessage('Running init-windows with cpp-lib template...');

    try {
      const modifiedConfig: Config = {
        ...config,
        root: this.root,
      };

      await initWindowsInternal([], modifiedConfig, {
        template: 'cpp-lib',
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

  // Step 8: Run codegen-windows
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
      const codegenDir = path.join(this.root, 'windows');
      if (await fs.exists(codegenDir)) {
        this.verboseMessage(
          'Windows directory exists, continuing despite codegen-windows warnings',
        );
      } else {
        throw new CodedError(
          'InvalidCodegenConfig',
          `Failed to run codegen-windows: ${error.message}`,
        );
      }
    }
  }

  // Step 9: Generate C++ stub files with Hello World methods
  private async generateStubFiles(): Promise<void> {
    this.verboseMessage('Generating C++ stub files...');

    const methods = await this.getHelloWorldMethods();
    const moduleName = await this.getFinalModuleName();

    const windowsDir = path.join(this.root, 'windows');
    const moduleDir = path.join(windowsDir, moduleName);

    // Ensure module directory exists
    await fs.mkdir(moduleDir, {recursive: true});

    const headerPath = path.join(moduleDir, `${moduleName}.h`);
    const cppPath = path.join(moduleDir, `${moduleName}.cpp`);

    // Store the actual project path for the success message
    this.actualProjectPath = path.join('windows', moduleName, moduleName);

    // Generate header file with Hello World methods
    const headerContent = await this.generateHeaderStub(moduleName, methods);
    await fs.writeFile(headerPath, headerContent);
    this.verboseMessage(
      `Generated header stub: ${headerPath} with ${methods.length} methods`,
    );

    // Generate cpp file with Hello World methods
    const cppContent = await this.generateCppStub(moduleName, methods);
    await fs.writeFile(cppPath, cppContent);
    this.verboseMessage(
      `Generated cpp stub: ${cppPath} with ${methods.length} methods`,
    );
  }

  private async getHelloWorldMethods(): Promise<MethodSignature[]> {
    return [
      {
        name: 'sayHello',
        returnType: 'Promise<string>',
        parameters: [{name: 'name', type: 'string'}],
      },
      {
        name: 'multiply',
        returnType: 'Promise<number>',
        parameters: [
          {name: 'a', type: 'number'},
          {name: 'b', type: 'number'},
        ],
      },
      {
        name: 'getRandomNumber',
        returnType: 'Promise<number>',
        parameters: [],
      },
      {
        name: 'isModuleReady',
        returnType: 'Promise<boolean>',
        parameters: [],
      },
    ];
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

  private async generateHeaderStub(
    moduleName: string,
    methods: MethodSignature[],
  ): Promise<string> {
    const methodDeclarations = methods
      .map(method => {
        const cppParams = method.parameters
          .map(p => {
            const cppType = this.mapTSToCppType(p.type);
            return `${cppType} ${p.name}`;
          })
          .join(', ');

        return `  REACT_METHOD(${method.name})
  void ${method.name}(${cppParams}${
          cppParams ? ', ' : ''
        }React::ReactPromise<${this.mapTSReturnTypeToCpp(
          method.returnType,
        )}> promise) noexcept;`;
      })
      .join('\n\n');

    return `#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native${moduleName}DataTypes.g.h")
  #include "codegen/Native${moduleName}DataTypes.g.h"
#endif
#include "codegen/Native${moduleName}Spec.g.h"

#include "NativeModules.h"

namespace winrt::${moduleName}Specs
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE(${moduleName})
struct ${moduleName}
{
  using ModuleSpec = ${moduleName}Codegen::${moduleName}Spec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

${methodDeclarations}

private:
  React::ReactContext m_context;
};

} // namespace winrt::${moduleName}Specs`;
  }

  private async generateCppStub(
    moduleName: string,
    methods: MethodSignature[],
  ): Promise<string> {
    const methodImplementations = methods
      .map(method => {
        const cppParams = method.parameters
          .map(p => {
            const cppType = this.mapTSToCppType(p.type);
            return `${cppType} ${p.name}`;
          })
          .join(', ');

        const implementation = this.generateMethodImplementation(method);

        return `void ${moduleName}::${method.name}(${cppParams}${
          cppParams ? ', ' : ''
        }React::ReactPromise<${this.mapTSReturnTypeToCpp(
          method.returnType,
        )}> promise) noexcept {
${implementation}
}`;
      })
      .join('\n\n');

    return `#include "${moduleName}.h"
#include <random>

namespace winrt::${moduleName}Specs {

void ${moduleName}::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

${methodImplementations}

} // namespace winrt::${moduleName}Specs
`;
  }

  private generateMethodImplementation(method: MethodSignature): string {
    switch (method.name) {
      case 'sayHello':
        return `  // Hello World implementation - demonstrates string handling
  try {
    std::string greeting = "Hello, " + name + "! Welcome to React Native Windows.";
    promise.Resolve(greeting);
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }`;

      case 'multiply':
        return `  // Reference multiply function - demonstrates numeric operations
  // Keep this as an example for new module developers
  try {
    double result = a * b;
    promise.Resolve(result);
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }`;

      case 'getRandomNumber':
        return `  // Example async method with no parameters
  try {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    double randomNum = dis(gen);
    promise.Resolve(randomNum);
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }`;

      case 'isModuleReady':
        return `  // Example boolean return method
  try {
    // Always return true to indicate the module is ready
    promise.Resolve(true);
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }`;

      default:
        return `  // TODO: Implement ${method.name}
  try {
    promise.Resolve(${this.generateDefaultValue(method.returnType)});
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }`;
    }
  }

  private mapTSToCppType(tsType: string): string {
    const typeMap: {[key: string]: string} = {
      string: 'std::string',
      number: 'double',
      boolean: 'bool',
      object: 'React::JSValue',
      any: 'React::JSValue',
    };

    return typeMap[tsType] || 'React::JSValue';
  }

  private mapTSReturnTypeToCpp(tsReturnType: string): string {
    // Handle Promise<T> types
    const promiseMatch = tsReturnType.match(/Promise<(.+)>/);
    if (promiseMatch) {
      return this.mapTSToCppType(promiseMatch[1]);
    }
    return this.mapTSToCppType(tsReturnType);
  }

  private generateDefaultValue(returnType: string): string {
    const promiseMatch = returnType.match(/Promise<(.+)>/);
    const innerType = promiseMatch ? promiseMatch[1] : returnType;

    if (innerType === 'string') {
      return '"example"';
    } else if (innerType === 'number') {
      return '0.0';
    } else if (innerType === 'boolean') {
      return 'true';
    } else {
      return 'React::JSValue{}';
    }
  }

  // Step 10: Verify build setup
  private async verifyBuild(): Promise<void> {
    if (this.options.skipBuild) {
      this.verboseMessage('Skipping build verification');
      return;
    }

    this.verboseMessage('Verifying build setup...');

    const windowsDir = path.join(this.root, 'windows');
    if (!(await fs.exists(windowsDir))) {
      this.verboseMessage(
        'No windows directory found, skipping build verification',
      );
      return;
    }

    const files = await fs.readdir(windowsDir);
    const slnFiles = files.filter(file => file.endsWith('.sln'));
    if (slnFiles.length === 0) {
      this.verboseMessage('No .sln file found, skipping build verification');
      return;
    }

    try {
      // Just check if MSBuild is available, don't actually build to keep setup fast
      execSync('where msbuild', {stdio: 'ignore'});
      this.verboseMessage('MSBuild found, project should be buildable');
    } catch {
      this.verboseMessage(
        'Warning: MSBuild not found, cannot verify build capability',
      );
    }
  }

  public async run(spinner: Ora, config: Config): Promise<void> {
    await this.validateEnvironment();
    spinner.text = 'Checking and creating spec file...';

    await this.checkAndCreateSpecFile();
    spinner.text = 'Updating package.json...';

    await this.updatePackageJsonCodegen();
    spinner.text = 'Cleaning and installing dependencies...';

    await this.cleanAndInstallDeps();
    spinner.text = 'Upgrading dependencies...';

    await this.upgradeDependencies();
    spinner.text = 'Setting up Windows library...';

    await this.runInitWindows(config);
    spinner.text = 'Running Windows codegen...';

    await this.runCodegenWindows(config);

    // Wait a bit for codegen files to be fully written to disk
    this.verboseMessage('Waiting for codegen files to be written...');
    await new Promise<void>(resolve => setTimeout(resolve, 1000));

    spinner.text = 'Generating C++ Hello World stub files...';

    await this.generateStubFiles();
    spinner.text = 'Verifying build setup...';

    await this.verifyBuild();

    spinner.succeed();
  }
}

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
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
      return value === undefined ? false : value;
  }
}

/**
 * Get the extra props to add to the setup-module-windows telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling `npx @react-native-community/cli setup-module-windows`.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
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
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
export async function setupModuleWindowsInternal(
  args: string[],
  config: Config,
  options: SetupModuleWindowsOptions,
) {
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

    // Get the actual module name and project paths for display
    const moduleName = await setup.getFinalModuleName();
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
      `🏗️  Native${moduleName}.ts - TurboModule spec file with Hello World methods`,
    );
    console.log(
      `💻 ${projectPaths.headerPath} - C++ header file with Hello World stubs`,
    );
    console.log(
      `⚙️  ${projectPaths.cppPath} - C++ implementation file with working examples`,
    );
    console.log('');
    console.log(chalk.bold('Hello World methods included:'));
    console.log('• sayHello(name) - Demonstrates string handling');
    console.log('• multiply(a, b) - Reference implementation for learning');
    console.log('• getRandomNumber() - Example async method');
    console.log('• isModuleReady() - Example boolean return method');
    console.log('');
    console.log(chalk.bold('Next steps:'));
    console.log("1. 📝 Test the Hello World methods or add your module's API");
    console.log('2. 🔧 Implement your specific business logic');
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
