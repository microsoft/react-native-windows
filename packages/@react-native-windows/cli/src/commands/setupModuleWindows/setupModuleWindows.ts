/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import chalk from 'chalk';
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
import {validateEnvironment, checkForExistingSpec} from './utils/validationUtils';
import {
  getFinalModuleName,
  updatePackageJsonCodegen,
} from './utils/moduleNameUtils';
import {
  runInitWindows,
  runCodegenWindows,
  getActualProjectPaths,
} from './utils/projectSetupUtils';
import {generateStubFiles} from './utils/templateGenerationUtils';

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(key: keyof SetupModuleWindowsOptions, value: any): any {
  // Do not add a default case here.
  // Strings risking PII should just return true if present, false otherwise.
  // All others should return the value (or false if undefined).
  switch (key) {
    case 'logging':
    case 'telemetry':
    case 'template':
      return value === undefined ? false : value; // Return value
  }
}

/**
 * Get the extra props to add to the `setup-module-windows` telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
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

  public getActualProjectPaths(): {headerPath: string; cppPath: string} {
    // Use the actual project path if available, otherwise fall back to calculated path
    if (this.actualProjectPath) {
      return {
        headerPath: `${this.actualProjectPath}.h`,
        cppPath: `${this.actualProjectPath}.cpp`,
      };
    }
    return getActualProjectPaths(this.root, this.actualModuleName);
  }

  public async run(spinner: Ora, config: Config): Promise<void> {
    await validateEnvironment(this.root, this.options.logging ?? false);
    spinner.text = 'Checking for TurboModule spec file...';

    const {actualModuleName} = await checkForExistingSpec(
      this.root,
      this.options.logging ?? false,
    );
    this.actualModuleName = actualModuleName;

    // Now begin the Windows setup process
    spinner.text = 'Updating package.json with codegen configuration...';
    await updatePackageJsonCodegen(this.root, this.options.logging ?? false);

    spinner.text = 'Running init-windows...';
    await runInitWindows(this.root, config, this.options);

    spinner.text = 'Running codegen-windows...';
    await runCodegenWindows(this.root, config, this.options);

    spinner.text = 'Generating C++ stub files...';
    const {actualProjectPath} = await generateStubFiles(
      this.root,
      this.actualModuleName,
      this.options.logging ?? false,
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
 * Internal function for use by other commands
 */
export async function setupModuleWindowsInternal(
  args: string[],
  config: Config,
  options: SetupModuleWindowsOptions,
): Promise<void> {
  // Suppress Node.js deprecation warnings during this command
  const originalNoWarnings = process.env.NODE_NO_WARNINGS;
  process.env.NODE_NO_WARNINGS = '1';

  try {
    const spinner = newSpinner('Setting up Windows module...');

    const setupModule = new SetupModuleWindows(config.root, {
      ...getDefaultOptions(config, setupModuleWindowsOptions),
      ...options,
    });

    await setupModule.run(spinner, config);
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