/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const { task, tscTask } = require('just-scripts');
const fs = require('fs');

// Use the shared base configuration
require('@rnw-scripts/just-task');

task('prepareBundle', () => {
  const file = 'windows/ReactUWPTestApp/Bundle';
  if (!fs.existsSync(file)) {
    fs.mkdirSync(file);
  }
});

task('ts', tscTask({ noEmit: true }));
