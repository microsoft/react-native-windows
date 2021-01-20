/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');

const {series, task, parallel} = require('just-scripts');

require('@rnw-scripts/just-task');

task('copyTemplateSettings', () =>
  fs.copyFileSync('./local.settings.example.json', './local.settings.json'),
);

if (fs.existsSync('./local.settings.json')) {
  task('build', series('ts'));
} else {
  task('build', parallel('ts', 'copyTemplateSettings'));
}

task('build', series('ts'));
