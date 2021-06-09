/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, logger} = require('just-scripts');
const os = require('os');

require('@rnw-scripts/just-task');

if (os.platform() !== 'win32') {
  task('test', () => {
    logger.warn('Tests are disabled on non-Windows platforms');
  });
}
