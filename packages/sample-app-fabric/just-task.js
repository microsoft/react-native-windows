/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task} = require('just-scripts');
const fs = require('fs');

// Use the shared base configuration
require('@rnw-scripts/just-task');

task('prepareBundle', () => {
  fs.mkdirSync('windows/SampleAppFabric/Bundle', {recursive: true});
});
