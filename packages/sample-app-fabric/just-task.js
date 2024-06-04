/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, series, parallel} = require('just-scripts');
const fs = require('fs');
const {execSync} = require('child_process');

// Use the shared base configuration
require('@rnw-scripts/just-task');

task('prepareBundle', () => {
  fs.mkdirSync('windows/SampleAppFabric/Bundle', {recursive: true});
});
