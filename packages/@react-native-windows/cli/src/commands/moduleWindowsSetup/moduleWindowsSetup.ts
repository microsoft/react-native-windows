/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import fs from '@react-native-windows/fs';
import shell from 'shelljs';
import chalk from 'chalk';
import {performance} from 'perf_hooks';
import {Ora} from 'ora';
import {execSync} from 'child_process';

import type {Command, Config} from '@react-native-community/cli-types';
import {Telemetry, CodedError} from '@react-native-windows/telemetry';
import {initWindowsInternal} from '../initWindows/initWindows';
import {codegenWindowsInternal} from '../codegenWindows/codegenWindows';

import {
  newSpinner,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import type {ModuleWindowsSetupOptions} from './moduleWindowsSetupOptions';
import {moduleWindowsSetupOptions} from './moduleWindowsSetupOptions';

export class ModuleWindowsSetup {
  private changesNecessary: boolean;

  public areChangesNeeded() {
    return this.changesNecessary;
  }

  constructor(
    readonly root: string,
    readonly options: ModuleWindowsSetupOptions,
  ) {
    this.changesNecessary = false;
  }

  private verboseMessage(message: any, verbose?: boolean) {
    if (verbose) {
      console.log(message);
    }
  }

  private async createSpecFile(
    packageJsonPath: string,
    packageJson: any,
    spinner: Ora,
  ): Promise<boolean> {
    const verbose = this.options.logging;
    let created = false;

    // Check if a spec file already exists
    const possibleSpecFiles = [
      path.join(this.root, 'src', `Native${packageJson.name}Spec.js`),
      path.join(this.root, 'src', `Native${packageJson.name}.js`),
      path.join(this.root, `Native${packageJson.name}Spec.js`),
      path.join(this.root, `Native${packageJson.name}.js`),
    ];

    const existingSpec = possibleSpecFiles.find(file => fs.existsSync(file));

    if (existingSpec) {
      spinner.info(
        `Spec file already exists: ${path.relative(this.root, existingSpec)}`,
      );
      return false;
    }

    // Create a basic TurboModule spec file
    const moduleName = this.options.name || packageJson.name;
    const specFileName = `Native${moduleName}Spec.js`;
    const srcDir = path.join(this.root, 'src');

    // Ensure src directory exists
    if (!fs.existsSync(srcDir)) {
      fs.mkdirSync(srcDir, {recursive: true});
    }

    const specFilePath = path.join(srcDir, specFileName);

    const specContent = `/**
 * @flow strict-local
 * @format
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import * as TurboModuleRegistry from 'react-native/Libraries/TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  // Add your module methods here
  // Example methods for different types:
  
  // Simple string method
  // +getString: (input: string) => string;
  
  // Number method
  // +getNumber: (input: number) => number;
  
  // Boolean method
  // +getBool: (input: boolean) => boolean;
  
  // Promise-based async method
  // +getStringAsync: (input: string) => Promise<string>;
  
  // Callback method
  // +getStringWithCallback: (input: string, callback: (result: string) => void) => void;
  
  // Method with no parameters
  // +doSomething: () => void;
  
  // Method that returns constants
  // +getConstants: () => {|
  //   SOME_CONSTANT: string;
  //   ANOTHER_CONSTANT: number;
  // |};
}

export default (TurboModuleRegistry.getEnforcing<Spec>('${moduleName}'): Spec);
`;

    fs.writeFileSync(specFilePath, specContent);
    this.verboseMessage(`Created spec file: ${specFilePath}`, verbose);
    spinner.info(
      `Created spec file: ${path.relative(this.root, specFilePath)}`,
    );
    created = true;

    return created;
  }

  private async updatePackageJsonCodegen(
    packageJsonPath: string,
    packageJson: any,
    spinner: Ora,
  ): Promise<boolean> {
    const verbose = this.options.logging;
    let updated = false;

    // Check if codegenConfig already exists
    if (packageJson.codegenConfig) {
      spinner.info('codegenConfig already exists in package.json');
      return false;
    }

    // Add codegenConfig
    const moduleName = this.options.name || packageJson.name;
    packageJson.codegenConfig = {
      name: moduleName,
      type: 'modules',
      jsSrcsDir: 'src',
      windows: {
        namespace: `${moduleName}Codegen`,
        separateDataTypes: true,
      },
    };

    fs.writeFileSync(
      packageJsonPath,
      JSON.stringify(packageJson, null, 2) + '\\n',
    );
    this.verboseMessage(`Updated package.json with codegenConfig`, verbose);
    spinner.info('Updated package.json with codegenConfig');
    updated = true;

    return updated;
  }

  private async cleanAndInstallDependencies(spinner: Ora): Promise<void> {
    const verbose = this.options.logging;

    // Remove node_modules if it exists
    const nodeModulesPath = path.join(this.root, 'node_modules');
    if (fs.existsSync(nodeModulesPath)) {
      this.verboseMessage('Removing node_modules directory...', verbose);
      shell.rm('-rf', nodeModulesPath);
      spinner.info('Removed node_modules directory');
    }

    // Run yarn install
    this.verboseMessage('Running yarn install...', verbose);
    spinner.text = 'Installing dependencies...';

    try {
      execSync('yarn install', {
        cwd: this.root,
        stdio: verbose ? 'inherit' : 'pipe',
      });
      spinner.info('Dependencies installed successfully');
    } catch (error) {
      throw new CodedError(
        'MSBuildError',
        `Failed to install dependencies: ${error}`,
      );
    }
  }

  private async upgradeDependencies(spinner: Ora): Promise<void> {
    const verbose = this.options.logging;

    spinner.text =
      'Upgrading React Native and React Native Windows to latest versions...';

    try {
      // Get latest versions
      const rnLatest = execSync('npm view react-native version', {
        encoding: 'utf8',
      }).trim();
      const rnwLatest = execSync('npm view react-native-windows version', {
        encoding: 'utf8',
      }).trim();

      this.verboseMessage(`Latest React Native: ${rnLatest}`, verbose);
      this.verboseMessage(`Latest React Native Windows: ${rnwLatest}`, verbose);

      // Update package.json
      const packageJsonPath = path.join(this.root, 'package.json');
      const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));

      // Update dependencies
      if (packageJson.dependencies) {
        if (packageJson.dependencies['react-native']) {
          packageJson.dependencies['react-native'] = `^${rnLatest}`;
        }
        if (packageJson.dependencies['react-native-windows']) {
          packageJson.dependencies['react-native-windows'] = `^${rnwLatest}`;
        }
      }

      // Update peerDependencies
      if (packageJson.peerDependencies) {
        if (packageJson.peerDependencies['react-native']) {
          packageJson.peerDependencies['react-native'] = `^${rnLatest}`;
        }
        if (packageJson.peerDependencies['react-native-windows']) {
          packageJson.peerDependencies[
            'react-native-windows'
          ] = `^${rnwLatest}`;
        }
      }

      fs.writeFileSync(
        packageJsonPath,
        JSON.stringify(packageJson, null, 2) + '\\n',
      );

      // Run yarn install again
      execSync('yarn install', {
        cwd: this.root,
        stdio: verbose ? 'inherit' : 'pipe',
      });

      spinner.info(
        `Upgraded to React Native ${rnLatest} and React Native Windows ${rnwLatest}`,
      );
    } catch (error) {
      // Don't fail the whole process if upgrade fails - continue with existing versions
      spinner.warn(`Failed to upgrade dependencies: ${error}`);
    }
  }

  private async generateCppStubs(spinner: Ora): Promise<void> {
    const verbose = this.options.logging;

    // Find the generated spec file
    const packageJsonPath = path.join(this.root, 'package.json');
    const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));

    if (!packageJson.codegenConfig) {
      spinner.info('No codegenConfig found - skipping C++ stub generation');
      return;
    }

    const codegenOutputDir =
      packageJson.codegenConfig.windows?.outputDirectory ?? 'codegen';
    const moduleName = this.options.name || packageJson.name;
    const specFileName = `Native${moduleName}Spec.g.h`;
    const specFilePath = path.join(this.root, codegenOutputDir, specFileName);

    if (!fs.existsSync(specFilePath)) {
      spinner.warn(`Generated spec file not found: ${specFilePath}`);
      return;
    }

    // Read the spec file to extract method signatures
    const specContent = fs.readFileSync(specFilePath, 'utf8');
    this.verboseMessage(`Reading spec file: ${specFilePath}`, verbose);

    // Parse method signatures from REACT_SHOW_METHOD_SPEC_ERRORS
    const methodMatches = specContent.matchAll(
      /REACT_SHOW_METHOD_SPEC_ERRORS\(\s*\d+,\s*"([^"]+)",\s*"([^"]*REACT_(?:SYNC_)?METHOD\([^)]+\)[^\n]*)/g,
    );

    const methods: Array<{name: string; signature: string}> = [];
    for (const match of methodMatches) {
      const methodName = match[1];
      const methodLine = match[2];

      // Extract the actual method signature
      const signatureMatch = methodLine.match(
        /REACT_(?:SYNC_)?METHOD\([^)]+\)\s*([^{]*)/,
      );
      if (signatureMatch) {
        methods.push({
          name: methodName,
          signature: signatureMatch[1].trim().replace(/\s+/g, ' '),
        });
      }
    }

    // Parse constant methods from REACT_SHOW_CONSTANT_SPEC_ERRORS
    const constantMatches = specContent.matchAll(
      /REACT_SHOW_CONSTANT_SPEC_ERRORS\(\s*\d+,\s*"([^"]+)",\s*"([^"]*REACT_GET_CONSTANTS\([^)]+\)[^\n]*)/g,
    );

    for (const match of constantMatches) {
      const constantName = match[1];
      const constantLine = match[2];

      // Extract the actual constant method signature
      const signatureMatch = constantLine.match(
        /REACT_GET_CONSTANTS\([^)]+\)\s*([^{]*)/,
      );
      if (signatureMatch) {
        methods.push({
          name: constantName.replace('_Constants', ''),
          signature: signatureMatch[1].trim().replace(/\s+/g, ' '),
        });
      }
    }

    if (methods.length === 0) {
      spinner.info('No methods found in spec file to generate stubs for');
      return;
    }

    this.verboseMessage(
      `Found ${methods.length} methods to generate stubs for`,
      verbose,
    );

    // Create C++ implementation file
    const namespace =
      packageJson.codegenConfig.windows?.namespace || `${moduleName}Codegen`;
    const headerFileName = `${moduleName}Module.h`;
    const implFileName = `${moduleName}Module.cpp`;

    // Generate header file
    const headerContent = this.generateHeaderFile(
      moduleName,
      namespace,
      methods,
    );
    const headerPath = path.join(this.root, 'windows', headerFileName);

    // Generate implementation file
    const implContent = this.generateImplementationFile(
      moduleName,
      namespace,
      methods,
    );
    const implPath = path.join(this.root, 'windows', implFileName);

    // Ensure windows directory exists
    const windowsDir = path.join(this.root, 'windows');
    if (!fs.existsSync(windowsDir)) {
      fs.mkdirSync(windowsDir, {recursive: true});
    }

    // Write files (only if they don't exist to avoid overwriting user changes)
    if (!fs.existsSync(headerPath)) {
      fs.writeFileSync(headerPath, headerContent);
      this.verboseMessage(`Created header file: ${headerPath}`, verbose);
      spinner.info(
        `Created header stub: ${path.relative(this.root, headerPath)}`,
      );
    } else {
      spinner.info(
        `Header file already exists: ${path.relative(this.root, headerPath)}`,
      );
    }

    if (!fs.existsSync(implPath)) {
      fs.writeFileSync(implPath, implContent);
      this.verboseMessage(`Created implementation file: ${implPath}`, verbose);
      spinner.info(
        `Created implementation stub: ${path.relative(this.root, implPath)}`,
      );
    } else {
      spinner.info(
        `Implementation file already exists: ${path.relative(
          this.root,
          implPath,
        )}`,
      );
    }
  }

  private generateHeaderFile(
    moduleName: string,
    namespace: string,
    methods: Array<{name: string; signature: string}>,
  ): string {
    const className = `${moduleName}Module`;
    const specClassName = `${moduleName}Spec`;

    return `#pragma once

#include "NativeModules.h"
#include "${moduleName}Spec.g.h"

namespace ${namespace}
{
    REACT_MODULE(${className})
    struct ${className}
    {
        using ModuleSpec = ${specClassName};

        REACT_INIT(Initialize)
        void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

${methods
  .map(
    method => `        REACT_METHOD(${method.name})
        ${method.signature};`,
  )
  .join('\n\n')}

    private:
        winrt::Microsoft::ReactNative::ReactContext m_reactContext{nullptr};
    };
}
`;
  }

  private generateImplementationFile(
    moduleName: string,
    namespace: string,
    methods: Array<{name: string; signature: string}>,
  ): string {
    const className = `${moduleName}Module`;
    const headerFileName = `${moduleName}Module.h`;

    const implementations = methods
      .map(method => {
        const signature = method.signature.replace(' noexcept', '');
        return `${signature} noexcept
{
    // TODO: Implement ${method.name}
    ${this.generateDefaultReturnValue(method.signature)}
}`;
      })
      .join('\n\n');

    return `#include "pch.h"
#include "${headerFileName}"

namespace ${namespace}
{
    void ${className}::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept
    {
        m_reactContext = reactContext;
    }

${implementations}

}
`;
  }

  private generateDefaultReturnValue(signature: string): string {
    if (signature.includes('void ')) {
      return '// No return value needed';
    } else if (signature.includes('bool ')) {
      return 'return false;';
    } else if (signature.includes('double ') || signature.includes('int ')) {
      return 'return 0;';
    } else if (signature.includes('std::string ')) {
      return 'return "";';
    } else if (signature.includes('Promise<')) {
      const promiseMatch = signature.match(
        /([^\s]+)\s+\w+\s*\([^)]*Promise<([^>]+)>/,
      );
      if (promiseMatch) {
        return `// promise.resolve(${this.getDefaultValueForType(
          promiseMatch[2],
        )});`;
      }
      return '// promise.resolve(...);';
    } else {
      return '// return default_value;';
    }
  }

  private getDefaultValueForType(type: string): string {
    if (type.includes('string')) return '""';
    if (type.includes('bool')) return 'false';
    if (
      type.includes('number') ||
      type.includes('double') ||
      type.includes('int')
    )
      return '0';
    return 'default_value';
  }

  public async run(spinner: Ora) {
    const verbose = this.options.logging;

    this.verboseMessage('Starting module-windows-setup...', verbose);

    const packageJsonPath = path.join(this.root, 'package.json');
    if (!fs.existsSync(packageJsonPath)) {
      throw new CodedError(
        'NoPackageJson',
        `package.json not found in ${this.root}. Please run this command from the root of a React Native project.`,
      );
    }

    const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));

    // Step 1: Create spec file if needed
    spinner.text = 'Checking for TurboModule spec file...';
    await this.createSpecFile(packageJsonPath, packageJson, spinner);

    // Step 2: Update package.json with codegenConfig if needed
    spinner.text = 'Updating package.json codegenConfig...';
    const updatedPackageJson = JSON.parse(
      fs.readFileSync(packageJsonPath, 'utf8'),
    );
    await this.updatePackageJsonCodegen(
      packageJsonPath,
      updatedPackageJson,
      spinner,
    );

    // Step 3 & 4: Clean and install dependencies
    await this.cleanAndInstallDependencies(spinner);

    // Step 5 & 6: Upgrade dependencies and install again
    await this.upgradeDependencies(spinner);

    // Step 7: Run init-windows with cpp-lib template
    spinner.text = 'Setting up Windows implementation...';
    try {
      const moduleName = this.options.name || packageJson.name;
      await initWindowsInternal(
        [],
        {root: this.root, project: {}, dependencies: {}} as Config,
        {
          template: 'cpp-lib',
          name: moduleName,
          overwrite: this.options.overwrite || false,
          logging: verbose,
        },
      );
      spinner.info('Windows implementation setup completed');
    } catch (error) {
      throw new CodedError(
        'MSBuildError',
        `Failed to setup Windows implementation: ${error}`,
      );
    }

    // Step 8: Run codegen-windows
    spinner.text = 'Running Windows codegen...';
    try {
      await codegenWindowsInternal(
        [],
        {root: this.root, project: {}, dependencies: {}} as Config,
        {logging: verbose},
      );
      spinner.info('Windows codegen completed');
    } catch (error) {
      throw new CodedError(
        'NeedCodegen',
        `Failed to run Windows codegen: ${error}`,
      );
    }

    // Step 9: Add C++ stub methods
    spinner.text = 'Generating C++ stub implementations...';
    await this.generateCppStubs(spinner);

    // Step 10: Build solution (optional - can be done manually)
    spinner.succeed('Module Windows setup completed successfully!');

    console.log(`
${chalk.green('✓')} Module Windows setup completed successfully!

Next steps:
1. Implement your TurboModule methods in the generated C++ files
2. Build the solution: Open windows/${packageJson.name || 'YourModule'}/${
      packageJson.name || 'YourModule'
    }.sln in Visual Studio
3. Test your module in a React Native Windows app

For more information, see: https://microsoft.github.io/react-native-windows/docs/native-modules
`);

    this.changesNecessary = true;
  }
}

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(
  key: keyof ModuleWindowsSetupOptions,
  value: any,
): any {
  // Do not add a default case here.
  // Strings risking PII should just return true if present, false otherwise.
  // All others should return the value (or false if undefined).
  switch (key) {
    case 'logging':
    case 'overwrite':
    case 'telemetry':
      return value === undefined ? false : value;
    case 'name':
      return value ? true : false; // Don't expose the actual name
  }
}

/**
 * Get the extra props to add to the telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling \`npx @react-native-community/cli module-windows-setup\`.
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
 * Sets up a React Native module for Windows development with New Architecture Fabric.
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
  const spinner = newSpinner('Setting up module for Windows development...');
  try {
    const setup = new ModuleWindowsSetup(config.root, options);
    await setup.run(spinner);
    const endTime = performance.now();

    console.log(
      `${chalk.green('Success:')} Module Windows setup completed. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
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
 * Sets up a React Native module for Windows development.
 */
export const moduleWindowsSetupCommand: Command = {
  name: 'module-windows-setup',
  description:
    'Sets up a React Native module for Windows development with New Architecture Fabric',
  func: moduleWindowsSetup,
  options: moduleWindowsSetupOptions,
};
