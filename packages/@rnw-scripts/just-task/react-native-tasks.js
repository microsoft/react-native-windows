/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {task} = require('just-scripts');

const copyRNLibraries = require('./copyRNLibraries');

task('copyRNLibraries', copyRNLibraries.copyTask(process.cwd()));
task('cleanRNLibraries', copyRNLibraries.cleanTask(process.cwd()));

task('flow-check', () => {
  require('child_process').execSync('npx flow check', {stdio: 'inherit'});
});
