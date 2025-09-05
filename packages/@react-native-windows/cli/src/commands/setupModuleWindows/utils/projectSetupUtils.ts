/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {Config} from '@react-native-community/cli-types';
import fs from '@react-native-windows/fs';
import path from 'path';

import {initWindowsInternal} from '../../initWindows/initWindows';
import {codegenWindowsInternal} from '../../codegenWindows/codegenWindows';
import type {SetupModuleWindowsOptions} from '../setupModuleWindowsOptions';
import {getActualModuleName} from './moduleNameUtils';

export async function runInitWindows(
  root: string,
  config: Config,
  options: SetupModuleWindowsOptions,
): Promise<void> {
  if (options.logging) {
    console.log('[SetupModuleWindows] Running init-windows...');
  }

  // Handle problematic example directory for cpp-lib template
  const exampleDir = path.join(root, 'example');
  const examplePackageJson = path.join(exampleDir, 'package.json');
  let temporarilyRenamed = false;
  let tempExampleDir = '';

  try {
    // If using cpp-lib template and example directory exists but doesn't have a proper package.json,
    // temporarily rename it to avoid template processing issues
    if (
      (options.template || 'cpp-lib') === 'cpp-lib' &&
      (await fs.exists(exampleDir)) &&
      !(await fs.exists(examplePackageJson))
    ) {
      tempExampleDir = path.join(root, 'example.temp.setup-module-windows');
      await fs.rename(exampleDir, tempExampleDir);
      temporarilyRenamed = true;
      if (options.logging) {
        console.log(
          '[SetupModuleWindows] Temporarily renamed example directory to avoid conflicts',
        );
      }
    }

    const modifiedConfig: Config = {
      ...config,
      root: root,
    };

    await initWindowsInternal([], modifiedConfig, {
      template: options.template || 'cpp-lib',
      logging: options.logging,
      telemetry: options.telemetry,
      overwrite: true,
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
  } finally {
    // Restore the temporarily renamed example directory
    if (temporarilyRenamed && tempExampleDir) {
      try {
        if (await fs.exists(tempExampleDir)) {
          await fs.rename(tempExampleDir, exampleDir);
          if (options.logging) {
            console.log('[SetupModuleWindows] Restored example directory');
          }
        }
      } catch (restoreError: any) {
        if (options.logging) {
          console.log(
            '[SetupModuleWindows] Warning: Failed to restore example directory:',
            restoreError.message,
          );
        }
      }
    }
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
      console.log(
        '[SetupModuleWindows] codegen-windows completed successfully',
      );
    }
  } catch (error: any) {
    // Check if codegen directory was created even with errors
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
  const moduleName = getActualModuleName(pkgJson.name || 'SampleModule');

  return {
    headerPath: `windows/${moduleName}/${moduleName}.h`,
    cppPath: `windows/${moduleName}/${moduleName}.cpp`,
  };
}
