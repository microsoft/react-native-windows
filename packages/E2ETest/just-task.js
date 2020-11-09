/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const { task, series, eslintTask, tscTask } = require('just-scripts');
const fs = require('fs');

task('eslint', eslintTask());
task('eslint:fix', eslintTask({ fix: true }));
task('prepareBundle', () => {
  const file = 'windows/ReactUWPTestApp/Bundle';
  if (!fs.existsSync(file)) {
    fs.mkdirSync(file);
  }
});

task('lint', series('eslint', tscTask()));
task('lint:fix', series('eslint:fix'));
