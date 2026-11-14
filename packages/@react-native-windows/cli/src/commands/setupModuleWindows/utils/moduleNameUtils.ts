/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import fs from '@react-native-windows/fs';

export function getModuleName(packageName: string): string {
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

export function getActualModuleName(packageName: string): string {
  // Similar to getModuleName but handles scoped packages differently
  // e.g., "@company/react-native-module" -> "CompanyReactNativeModule"
  if (packageName.startsWith('@')) {
    return packageName
      .replace('@', '')
      .replace(/[/-]/g, ' ')
      .split(' ')
      .filter(word => word.length > 0)
      .map(word => word.charAt(0).toUpperCase() + word.slice(1).toLowerCase())
      .join('');
  }
  return getModuleName(packageName);
}

export async function getFinalModuleName(
  root: string,
  actualModuleName?: string,
): Promise<string> {
  if (actualModuleName) {
    return actualModuleName;
  }

  const packageJsonPath = path.join(root, 'package.json');
  const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));
  return getActualModuleName(pkgJson.name || 'SampleModule');
}

export async function updatePackageJsonCodegen(
  root: string,
  logging: boolean,
): Promise<void> {
  if (logging) {
    console.log(
      '[SetupModuleWindows] Checking and updating package.json codegen configuration...',
    );
  }

  const packageJsonPath = path.join(root, 'package.json');
  const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));

  if (!pkgJson.codegenConfig) {
    const moduleName = getActualModuleName(pkgJson.name || 'SampleModule');

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
    if (logging) {
      console.log('[SetupModuleWindows] Added codegenConfig to package.json');
    }
  } else {
    if (logging) {
      console.log(
        '[SetupModuleWindows] codegenConfig already exists in package.json',
      );
    }
  }
}
