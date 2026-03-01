#!/usr/bin/env node

/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// Ensure the package is built from latest source before running
const {execSync} = require('child_process');
execSync('npx tsc -p . --outDir lib-commonjs', {
  cwd: __dirname,
  stdio: 'inherit',
});

require('source-map-support').install();
require('./lib-commonjs/integrateRN.js');
