/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {parallel, series, task} = require('just-scripts');

// Use the shared base configuration
require('@rnw-scripts/just-task');

task('lint', parallel(series('prettier', 'eslint'), 'depcheck', 'ts'));
