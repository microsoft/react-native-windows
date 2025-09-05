/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import {execSync} from 'child_process';
import glob from 'glob';

import fs from '@react-native-windows/fs';
import {CodedError} from '@react-native-windows/telemetry';

export async function validateEnvironment(
  root: string,
  logging: boolean,
): Promise<void> {
  if (logging) {
    console.log('[SetupModuleWindows] Validating environment...');
  }

  // Check if package.json exists
  const packageJsonPath = path.join(root, 'package.json');
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
    if (logging) {
      console.log(`[SetupModuleWindows] Project name: ${pkgJson.name}`);
    }
  } catch (error: any) {
    if (error.code === 'NoProjectName') {
      throw error;
    }
    throw new CodedError('NoPackageJson', 'package.json is not valid JSON.');
  }

  // Check if yarn is available
  try {
    execSync('yarn --version', {stdio: 'ignore'});
    if (logging) {
      console.log('[SetupModuleWindows] Yarn found');
    }
  } catch {
    throw new CodedError(
      'Unknown',
      'Yarn is required but not found. Please install Yarn first.',
    );
  }
}

export async function checkForExistingSpec(
  root: string,
  logging: boolean,
): Promise<{validSpecFiles: string[]; actualModuleName?: string}> {
  if (logging) {
    console.log('[SetupModuleWindows] Checking for TurboModule spec file...');
  }

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
      cwd: root,
      ignore: ['**/node_modules/**', '**/build/**', '**/dist/**'],
    });
    specFiles.push(...matches);
  }

  // Remove duplicates and filter for actual TurboModule specs
  const uniqueSpecFiles = Array.from(new Set(specFiles));
  const validSpecFiles = await filterValidSpecFiles(uniqueSpecFiles, root);

  if (validSpecFiles.length === 0) {
    throw new CodedError(
      'NoTurboModuleSpec',
      'Create Spec File - TurboModule spec file not found. Please create a TurboModule spec file before running setup-module-windows.',
    );
  } else {
    if (logging) {
      console.log(
        `[SetupModuleWindows] Found valid spec file(s): ${validSpecFiles.join(', ')}`,
      );
    }
    const actualModuleName = await extractModuleNameFromExistingSpec(
      validSpecFiles[0],
      root,
    );
    return {validSpecFiles, actualModuleName};
  }
}

async function filterValidSpecFiles(
  specFiles: string[],
  root: string,
): Promise<string[]> {
  const validFiles: string[] = [];

  for (const file of specFiles) {
    try {
      const filePath = path.join(root, file);
      const content = await fs.readFile(filePath, 'utf8');

      if (isValidTurboModuleSpec(content)) {
        validFiles.push(file);
      }
    } catch (error) {
      // Skip files that can't be read
      continue;
    }
  }

  return validFiles;
}

function isValidTurboModuleSpec(content: string): boolean {
  // Check for TurboModule spec patterns
  const turboModulePatterns = [
    /TurboModule/,
    /export\s+interface\s+Spec/,
    /export\s+default\s+TurboModuleRegistry/,
  ];

  return turboModulePatterns.some(pattern => pattern.test(content));
}

async function extractModuleNameFromExistingSpec(
  specFile: string,
  root: string,
): Promise<string | undefined> {
  try {
    const filePath = path.join(root, specFile);
    const content = await fs.readFile(filePath, 'utf8');

    // Try to extract module name from export default TurboModuleRegistry.get<Spec>('ModuleName')
    const registryMatch = content.match(
      /TurboModuleRegistry\.get(?:Enforcing)?<[^>]+>\(\s*['"`]([^'"`]+)['"`]\s*\)/,
    );
    if (registryMatch) {
      return registryMatch[1];
    }

    // Fallback: extract from filename (remove Native prefix and file extension)
    const fileName = path.basename(specFile, path.extname(specFile));
    if (fileName.startsWith('Native')) {
      return fileName.slice(6); // Remove "Native" prefix
    }

    return fileName;
  } catch (error) {
    return undefined;
  }
}