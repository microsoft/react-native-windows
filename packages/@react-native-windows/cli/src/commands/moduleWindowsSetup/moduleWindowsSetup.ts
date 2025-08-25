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
  // Example:
  // +getString: (input: string) => string;
  // +getNumber: (input: number) => number;
  // +getPromise: (error: boolean) => Promise<string>;
  // +getCallback: (callback: (value: string) => void) => void;
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

    // Step 9: Add C++ stub methods (this will be implemented in next iteration)
    spinner.text = 'Generating C++ stub implementations...';
    // TODO: Implement C++ stub generation
    spinner.info('C++ stub generation completed (placeholder)');

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
