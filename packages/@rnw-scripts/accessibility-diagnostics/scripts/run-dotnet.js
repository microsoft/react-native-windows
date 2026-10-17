/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

const {spawnSync} = require('child_process');
const path = require('path');

const command = process.argv[2];
const root = path.resolve(__dirname, '..');
const solution = path.join(root, 'AccessibilityDiagnostics.sln');
const tests = path.join(
  root,
  'tests',
  'Microsoft.ReactNative.AccessibilityDiagnostics.Core.Tests',
  'Microsoft.ReactNative.AccessibilityDiagnostics.Core.Tests.csproj',
);

if (process.platform !== 'win32') {
  console.log(`Skipping Windows-only accessibility diagnostics ${command}.`);
  process.exit(0);
}

const commands = {
  build: ['build', solution, '--configuration', 'Release'],
  clean: ['clean', solution, '--configuration', 'Release'],
  lint: ['format', solution, '--verify-no-changes'],
  'lint:fix': ['format', solution],
  test: ['test', tests, '--configuration', 'Release'],
};

const args = commands[command];
if (!args) {
  console.error(`Unknown command: ${command}`);
  process.exit(2);
}

const result = spawnSync('dotnet', args, {cwd: root, stdio: 'inherit'});
if (result.error) {
  console.error(`Failed to run dotnet: ${result.error.message}`);
  process.exit(3);
}

process.exit(result.status ?? 3);
