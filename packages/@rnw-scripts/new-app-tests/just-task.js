/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {parallel, task} = require('just-scripts');

require('@rnw-scripts/just-task');

task('lint', parallel('eslint', 'depcheck', 'ts'));
