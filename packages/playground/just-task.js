/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const fs = require('fs');
const {task} = require('just-scripts');

// Use the shared base configuration
require('@rnw-scripts/just-task');

task('prepareBundleWin32', () => {
  fs.mkdirSync('windows/playground-win32/Bundle/Samples', {recursive: true});
});
