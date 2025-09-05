/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import chalk from 'chalk';
import {performance} from 'perf_hooks';
import {Ora} from 'ora';

import type {Command, Config} from '@react-native-community/cli-types';
import {Telemetry} from '@react-native-windows/telemetry';

import {
  newSpinner,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import type {SetupModuleWindowsOptions} from './setupModuleWindowsOptions';
import {setupModuleWindowsOptions} from './setupModuleWindowsOptions';
import {validateEnvironment, checkForExistingSpec} from './validationUtils';
import {
  getFinalModuleName,
  updatePackageJsonCodegen,
  getActualModuleName,
} from './moduleNameUtils';
import {
  cleanAndInstallDeps,
  runInitWindows,
  runCodegenWindows,
  getActualProjectPaths,
} from './projectSetupUtils';
import {generateStubFiles} from './templateGenerationUtils';

export class SetupModuleWindows {
  private actualModuleName?: string;
  private actualProjectPath?: string;
  public root: string;
  public options: SetupModuleWindowsOptions;

  constructor(root: string, options: SetupModuleWindowsOptions) {
    this.root = root;
    this.options = options;
  }

  public getActualProjectPaths(): {headerPath: string; cppPath: string} {
    return getActualProjectPaths(this.root, this.actualModuleName);
  }

  public async run(spinner: Ora, config: Config): Promise<void> {
    await validateEnvironment(this.root, this.options.logging);
    spinner.text = 'Checking for TurboModule spec file...';

    const {validSpecFiles, actualModuleName} = await checkForExistingSpec(
      this.root,
      this.options.logging,
    );
    this.actualModuleName = actualModuleName;

    // Now begin the Windows setup process
    spinner.text = 'Updating package.json with codegen configuration...';
    await updatePackageJsonCodegen(this.root, this.options.logging);

    spinner.text = 'Installing dependencies...';
    await cleanAndInstallDeps(this.root, this.options.logging);

    spinner.text = 'Running init-windows...';
    await runInitWindows(this.root, config, this.options);

    spinner.text = 'Running codegen-windows...';
    await runCodegenWindows(this.root, config, this.options);

    spinner.text = 'Generating C++ stub files...';
    const {actualProjectPath} = await generateStubFiles(
      this.root,
      this.actualModuleName,
      this.options.logging,
    );
    this.actualProjectPath = actualProjectPath;

    spinner.succeed();

    // Print success message with helpful information
    const moduleName = await getFinalModuleName(this.root, this.actualModuleName);
    const paths = this.getActualProjectPaths();

    console.log(
      `\n${chalk.green('Success!')} Windows module setup completed.\n`,
    );

    console.log(`${chalk.bold('Module Name:')} ${moduleName}`);
    console.log(`${chalk.bold('Generated Files:')}`);
    console.log(`  • ${paths.headerPath}`);
    console.log(`  • ${paths.cppPath}`);
    console.log(`  • codegen/Native${moduleName}Spec.g.h`);

    console.log(`\n${chalk.bold('Next Steps:')}`);
    console.log('1. Implement your module methods in the generated C++ files');
    console.log('2. Add your module to the React Native Windows project');
    console.log(
      '3. Build and test your module using the provided Hello World example',
    );

    console.log(
      `\n${chalk.blue('Learn More:')} https://microsoft.github.io/react-native-windows/docs/native-platform`,
    );
  }
}

/**
 * Function to setup Windows support for React Native community modules.
 */
export async function setupModuleWindows(
  args: string[],
  config: Config,
  options: SetupModuleWindowsOptions,
) {
  const startTime = performance.now();
  const telemetrySession = startTelemetrySession('setupModuleWindows');

  // Suppress Node.js deprecation warnings during this command
  const originalNoWarnings = process.env.NODE_NO_WARNINGS;
  process.env.NODE_NO_WARNINGS = '1';

  try {
    const spinner = newSpinner('Setting up Windows module...');

    const setupModule = new SetupModuleWindows(config.root, {
      ...getDefaultOptions(args),
      ...options,
    });

    await setupModule.run(spinner, config);

    const endTime = performance.now();

    Telemetry.client?.trackEvent({
      name: 'setup-module-windows',
      properties: {
        durationInMs: endTime - startTime,
        template: options.template || 'cpp-lib',
      },
    });
  } catch (e) {
    if (!options.telemetry) {
      console.log(
        `Telemetry is disabled. The following error will not be sent to Microsoft: ${e}`,
      );
    }
    Telemetry.client?.trackException({exception: e as Error});
    setExitProcessWithError(e);
  } finally {
    endTelemetrySession(telemetrySession);
    // Restore original NODE_NO_WARNINGS setting
    if (originalNoWarnings !== undefined) {
      process.env.NODE_NO_WARNINGS = originalNoWarnings;
    } else {
      delete process.env.NODE_NO_WARNINGS;
    }
  }
}

/**
 * Internal function for use by other commands
 */
export async function setupModuleWindowsInternal(
  args: string[],
  config: Config,
  options: SetupModuleWindowsOptions,
): Promise<void> {
  await setupModuleWindows(args, config, options);
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