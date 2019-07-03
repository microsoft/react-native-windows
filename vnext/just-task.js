/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  task,
  copyTask,
  series,
  condition,
  option,
  argv,
  tscTask,
  eslintTask,
  cleanTask,
} = require('just-scripts');
const libPath = path.resolve(process.cwd(), 'lib');
const srcPath = path.resolve(process.cwd(), 'src');

option('production');
option('clean');

task('eslint', () => {
  return eslintTask();
});
task('copyFlowFiles', () => {
  return copyTask(['src/**/*.js'], '.');
});
task('ts', () => {
  return tscTask({
    pretty: true,
    ...(argv().production && {
      inlineSources: true,
      sourceRoot: path.relative(libPath, srcPath),
    }),
    target: 'es5',
    module: 'commonjs',
  });
});
task('clean', () => {
  return cleanTask(
    ['Libraries', 'RNTester', 'lib'].map(p => path.join(process.cwd(), p)),
  );
});

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'eslint',
    'copyFlowFiles',
    'ts',
  ),
);
