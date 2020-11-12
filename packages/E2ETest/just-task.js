/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const { task, parallel, series, eslintTask, tscTask } = require('just-scripts');
const fs = require('fs');

task('eslint', eslintTask());
task('eslint:fix', eslintTask({ fix: true }));
task('prepareBundle', () => {
  const file = 'windows/ReactUWPTestApp/Bundle';
  if (!fs.existsSync(file)) {
    fs.mkdirSync(file);
  }
});

task('ts', tscTask({ noEmit: true }));

task('lint', parallel('eslint', 'ts'));
task('lint:fix', series('eslint:fix'));
