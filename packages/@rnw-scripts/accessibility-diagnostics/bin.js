#!/usr/bin/env node

/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

const {spawnSync} = require('child_process');
const path = require('path');

if (process.platform !== 'win32') {
  console.error('rnw-a11y is only supported on Windows.');
  process.exit(2);
}

const project = path.join(
  __dirname,
  'src',
  'Microsoft.ReactNative.AccessibilityDiagnostics.Cli',
  'Microsoft.ReactNative.AccessibilityDiagnostics.Cli.csproj',
);
const result = spawnSync(
  'dotnet',
  [
    'run',
    '--project',
    project,
    '--configuration',
    'Release',
    '--',
    ...process.argv.slice(2),
  ],
  {stdio: 'inherit'},
);

if (result.error) {
  console.error(`Failed to start rnw-a11y: ${result.error.message}`);
  process.exit(3);
}

process.exit(result.status ?? 3);
