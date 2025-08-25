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
import type {ModuleWindowsSetupOptions} from './moduleWindowsSetupOptions';
import {moduleWindowsSetupOptions} from './moduleWindowsSetupOptions';

export class ModuleWindowsSetup {
  constructor(readonly root: string, readonly options: ModuleWindowsSetupOptions) {}

  private verboseMessage(message: any) {
    if (this.options.logging) {
      console.log(`[ModuleWindowsSetup] ${message}`);
    }
  }

  private getModuleName(packageName: string): string {
    // Convert package name to PascalCase module name
    // e.g., "react-native-webview" -> "ReactNativeWebview"
    // e.g., "@react-native-community/slider" -> "ReactNativeCommunitySlider"
    return packageName
      .replace(/[@\/\-]/g, ' ')
      .split(' ')
      .filter(word => word.length > 0)
      .map(word => word.charAt(0).toUpperCase() + word.slice(1).toLowerCase())
      .join('');
  }

  private async checkAndCreateSpecFile(): Promise<void> {
    this.verboseMessage('Checking for TurboModule spec file...');
    
    const specPattern = '**/Native*.[jt]s';
    const specFiles = glob.sync(specPattern, {cwd: this.root});
    
    if (specFiles.length === 0) {
      this.verboseMessage('No spec file found, creating default TurboModule spec...');
      await this.createDefaultSpecFile();
    } else {
      this.verboseMessage(`Found spec file(s): ${specFiles.join(', ')}`);
    }
  }

  private async createDefaultSpecFile(): Promise<void> {
    const pkgJson = JSON.parse(await fs.readFile(path.join(this.root, 'package.json'), 'utf8'));
    const moduleName = this.getModuleName(pkgJson.name || 'SampleModule');
    
    const specContent = `/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  // Add your module methods here
  // Example:
  // getString(value: string): Promise<string>;
  // getNumber(value: number): Promise<number>;
  // getBoolean(value: boolean): Promise<boolean>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('${moduleName}');
`;

    const specPath = path.join(this.root, `Native${moduleName}.ts`);
    await fs.writeFile(specPath, specContent);
    this.verboseMessage(`Created spec file: ${specPath}`);
  }

  private async updatePackageJsonCodegen(): Promise<void> {
    this.verboseMessage('Checking and updating package.json codegen configuration...');
    
    const packageJsonPath = path.join(this.root, 'package.json');
    const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));
    
    if (!pkgJson.codegenConfig) {
      const moduleName = this.getModuleName(pkgJson.name || 'SampleModule');
      
      pkgJson.codegenConfig = {
        name: moduleName,
        type: 'modules',
        jsSrcsDir: '.',
        windows: {
          namespace: `${moduleName}Specs`,
          outputDirectory: 'codegen',
          generators: ['modulesWindows']
        }
      };
      
      await fs.writeFile(packageJsonPath, JSON.stringify(pkgJson, null, 2));
      this.verboseMessage('Added codegenConfig to package.json');
    } else {
      this.verboseMessage('codegenConfig already exists in package.json');
    }
  }

  private async cleanAndInstallDeps(): Promise<void> {
    this.verboseMessage('Cleaning node_modules and reinstalling dependencies...');
    
    const nodeModulesPath = path.join(this.root, 'node_modules');
    if (await fs.exists(nodeModulesPath)) {
      await fs.rm(nodeModulesPath, {recursive: true, force: true});
      this.verboseMessage('Removed node_modules');
    }
    
    execSync('yarn install', {cwd: this.root, stdio: 'inherit'});
    this.verboseMessage('Dependencies installed');
  }

  private async upgradeDependencies(): Promise<void> {
    if (this.options.skipDeps) {
      this.verboseMessage('Skipping dependency upgrades');
      return;
    }
    
    this.verboseMessage('Upgrading React Native and React Native Windows to latest versions...');
    
    try {
      // Get latest versions
      const rnLatest = execSync('npm view react-native version', {encoding: 'utf8'}).trim();
      const rnwLatest = execSync('npm view react-native-windows version', {encoding: 'utf8'}).trim();
      
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
      
      // Install updated dependencies
      execSync('yarn install', {cwd: this.root, stdio: 'inherit'});
      
    } catch (error) {
      this.verboseMessage(`Warning: Could not upgrade dependencies: ${error}`);
    }
  }

  private async runInitWindows(config: Config): Promise<void> {
    this.verboseMessage('Running init-windows with cpp-lib template...');
    
    try {
      await initWindowsInternal([], config, {
        template: 'cpp-lib',
        overwrite: true,
        logging: this.options.logging,
        telemetry: this.options.telemetry
      });
      this.verboseMessage('init-windows completed successfully');
    } catch (error) {
      throw new CodedError('InitWindowsFailed', `Failed to run init-windows: ${error}`);
    }
  }

  private async runCodegenWindows(config: Config): Promise<void> {
    this.verboseMessage('Running codegen-windows...');
    
    try {
      await codegenWindowsInternal([], config, {
        logging: this.options.logging,
        telemetry: this.options.telemetry
      });
      this.verboseMessage('codegen-windows completed successfully');
    } catch (error) {
      throw new CodedError('CodegenWindowsFailed', `Failed to run codegen-windows: ${error}`);
    }
  }

  private async generateStubFiles(): Promise<void> {
    this.verboseMessage('Generating C++ stub files...');
    
    const codegenDir = path.join(this.root, 'codegen');
    if (!(await fs.exists(codegenDir))) {
      this.verboseMessage('No codegen directory found, skipping stub generation');
      return;
    }
    
    const files = await fs.readdir(codegenDir);
    const specFiles = files.filter(file => file.endsWith('Spec.g.h'));
    
    for (const specFile of specFiles) {
      const specName = specFile.replace('Spec.g.h', '');
      const windowsDir = path.join(this.root, 'windows');
      const headerPath = path.join(windowsDir, `${specName}.h`);
      const cppPath = path.join(windowsDir, `${specName}.cpp`);
      
      if (!(await fs.exists(windowsDir))) {
        await fs.mkdir(windowsDir, {recursive: true});
      }
      
      // Generate header file
      const headerContent = this.generateHeaderStub(specName);
      if (!(await fs.exists(headerPath))) {
        await fs.writeFile(headerPath, headerContent);
        this.verboseMessage(`Generated header stub: ${headerPath}`);
      }
      
      // Generate cpp file
      const cppContent = this.generateCppStub(specName);
      if (!(await fs.exists(cppPath))) {
        await fs.writeFile(cppPath, cppContent);
        this.verboseMessage(`Generated cpp stub: ${cppPath}`);
      }
    }
  }

  private generateHeaderStub(moduleName: string): string {
    return `#pragma once

#include <${moduleName}Spec.g.h>
#include <NativeModules.h>

namespace ${moduleName}Specs {

REACT_MODULE(${moduleName})
struct ${moduleName} {
  using ModuleSpec = ${moduleName}Spec;
  
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;
  
  // TODO: Add your method implementations here
  // Example:
  // REACT_METHOD(getString)
  // void getString(std::string value, React::ReactPromise<std::string> promise) noexcept;
};

} // namespace ${moduleName}Specs
`;
  }

  private generateCppStub(moduleName: string): string {
    return `#include "${moduleName}.h"

namespace ${moduleName}Specs {

void ${moduleName}::Initialize(React::ReactContext const &reactContext) noexcept {
  // TODO: Initialize your module
}

// TODO: Implement your methods here
// Example:
// void ${moduleName}::getString(std::string value, React::ReactPromise<std::string> promise) noexcept {
//   promise.Resolve(value);
// }

} // namespace ${moduleName}Specs
`;
  }

  private async verifyBuild(): Promise<void> {
    if (this.options.skipBuild) {
      this.verboseMessage('Skipping build verification');
      return;
    }
    
    this.verboseMessage('Verifying build...');
    
    const windowsDir = path.join(this.root, 'windows');
    if (!(await fs.exists(windowsDir))) {
      this.verboseMessage('No windows directory found, skipping build verification');
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
      this.verboseMessage('Warning: MSBuild not found, cannot verify build capability');
    }
  }

  public async run(spinner: Ora, config: Config): Promise<void> {
    await this.checkAndCreateSpecFile();
    spinner.text = 'Updating package.json...';
    
    await this.updatePackageJsonCodegen();
    spinner.text = 'Cleaning and installing dependencies...';
    
    await this.cleanAndInstallDeps();
    spinner.text = 'Upgrading dependencies...';
    
    await this.upgradeDependencies();
    spinner.text = 'Running yarn install...';
    
    execSync('yarn install', {cwd: this.root, stdio: 'inherit'});
    spinner.text = 'Setting up Windows library...';
    
    await this.runInitWindows(config);
    spinner.text = 'Running Windows codegen...';
    
    await this.runCodegenWindows(config);
    spinner.text = 'Generating C++ stub files...';
    
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
function optionSanitizer(key: keyof ModuleWindowsSetupOptions, value: any): any {
  switch (key) {
    case 'logging':
    case 'telemetry':
    case 'skipDeps':
    case 'skipBuild':
      return value === undefined ? false : value;
  }
}

/**
 * Get the extra props to add to the `module-windows-setup` telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling `npx @react-native-community/cli module-windows-setup`.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function moduleWindowsSetup(
  args: string[],
  config: Config,
  options: ModuleWindowsSetupOptions,
) {
  await startTelemetrySession(
    'module-windows-setup',
    config,
    options,
    getDefaultOptions(config, moduleWindowsSetupOptions),
    optionSanitizer,
  );

  let moduleWindowsSetupError: Error | undefined;
  try {
    await moduleWindowsSetupInternal(args, config, options);
  } catch (ex) {
    moduleWindowsSetupError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(moduleWindowsSetupError);
  }

  await endTelemetrySession(moduleWindowsSetupError, getExtraProps);
  setExitProcessWithError(options.logging, moduleWindowsSetupError);
}

/**
 * Sets up Windows support for React Native community modules.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
export async function moduleWindowsSetupInternal(
  args: string[],
  config: Config,
  options: ModuleWindowsSetupOptions,
) {
  const startTime = performance.now();
  const spinner = newSpinner('Setting up Windows support for React Native module...');
  
  try {
    const setup = new ModuleWindowsSetup(config.root, options);
    await setup.run(spinner, config);
    const endTime = performance.now();

    console.log(
      `${chalk.green('Success:')} Windows module setup completed! (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    console.log('');
    console.log(chalk.bold('Next steps:'));
    console.log('1. Update the generated spec file with your module\'s interface');
    console.log('2. Implement the methods in the generated C++ stub files');
    console.log('3. Build your project to verify everything works');
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
  }
}

/**
 * Sets up Windows support for React Native community modules.
 */
export const moduleWindowsSetupCommand: Command = {
  name: 'module-windows-setup',
  description: 'Streamlined setup of Windows support for React Native community modules',
  func: moduleWindowsSetup,
  options: moduleWindowsSetupOptions,
};