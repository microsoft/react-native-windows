/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {execSync} from 'child_process';
import type {Config} from '@react-native-community/cli-types';

import {initWindowsInternal} from '../initWindows/initWindows';
import {codegenWindowsInternal} from '../codegenWindows/codegenWindows';
import type {SetupModuleWindowsOptions} from './setupModuleWindowsOptions';
import {getFinalModuleName} from './moduleNameUtils';

export async function cleanAndInstallDeps(
  root: string,
  logging: boolean,
): Promise<void> {
  if (logging) {
    console.log('[SetupModuleWindows] Cleaning and installing dependencies...');
  }

  try {
    // Clean install to ensure fresh dependencies
    execSync('yarn install --check-files', {
      cwd: root,
      stdio: logging ? 'inherit' : 'ignore',
    });
    if (logging) {
      console.log('[SetupModuleWindows] Dependencies installed successfully');
    }
  } catch (error: any) {
    if (logging) {
      console.log(
        '[SetupModuleWindows] Failed to install dependencies:',
        error.message,
      );
    }
    throw error;
  }
}

export async function runInitWindows(
  root: string,
  config: Config,
  options: SetupModuleWindowsOptions,
): Promise<void> {
  if (options.logging) {
    console.log('[SetupModuleWindows] Running init-windows...');
  }

  try {
    const modifiedConfig: Config = {
      ...config,
      root: root,
    };

    await initWindowsInternal([], modifiedConfig, {
      template: options.template || 'cpp-lib',
      logging: options.logging,
      telemetry: options.telemetry,
      overwrite: false,
      namespace: '',
    });
    if (options.logging) {
      console.log('[SetupModuleWindows] init-windows completed successfully');
    }
  } catch (error: any) {
    if (options.logging) {
      console.log('[SetupModuleWindows] init-windows failed:', error.message);
    }
    throw error;
  }
}

export async function runCodegenWindows(
  root: string,
  config: Config,
  options: SetupModuleWindowsOptions,
): Promise<void> {
  if (options.logging) {
    console.log('[SetupModuleWindows] Running codegen-windows...');
  }

  try {
    const modifiedConfig: Config = {
      ...config,
      root: root,
    };

    await codegenWindowsInternal([], modifiedConfig, {
      logging: options.logging,
      telemetry: options.telemetry,
    });
    if (options.logging) {
      console.log('[SetupModuleWindows] codegen-windows completed successfully');
    }
  } catch (error: any) {
    // Check if codegen directory was created even with errors
    const fs = require('@react-native-windows/fs');
    const path = require('path');
    const codegenDir = path.join(root, 'codegen');
    if (await fs.exists(codegenDir)) {
      if (options.logging) {
        console.log(
          '[SetupModuleWindows] codegen-windows completed with warnings, but codegen directory was created',
        );
      }
      return; // Continue execution
    }
    if (options.logging) {
      console.log(
        '[SetupModuleWindows] codegen-windows failed:',
        error.message,
      );
    }
    throw error;
  }
}

export function getActualProjectPaths(
  root: string,
  actualModuleName?: string,
): {headerPath: string; cppPath: string} {
  const path = require('path');
  const fs = require('@react-native-windows/fs');
  
  // If we have an actual module name, use it
  if (actualModuleName) {
    return {
      headerPath: `windows/${actualModuleName}/${actualModuleName}.h`,
      cppPath: `windows/${actualModuleName}/${actualModuleName}.cpp`,
    };
  }

  // Otherwise, derive from package name
  const packageJsonPath = path.join(root, 'package.json');
  const pkgJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));
  const {getActualModuleName} = require('./moduleNameUtils');
  const moduleName = getActualModuleName(pkgJson.name || 'SampleModule');

  return {
    headerPath: `windows/${moduleName}/${moduleName}.h`,
    cppPath: `windows/${moduleName}/${moduleName}.cpp`,
  };
}