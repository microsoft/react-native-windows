/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {task} = require('just-scripts');

const copyRNLibaries = require('../../../packages/react-native-windows/Scripts/copyRNLibraries');

task('copyRNLibraries', copyRNLibaries.copyTask(process.cwd()));
task('cleanRNLibraries', copyRNLibaries.cleanTask(process.cwd()));

task('flow-check', () => {
  require('child_process').execSync('npx flow check', {stdio: 'inherit'});
});
