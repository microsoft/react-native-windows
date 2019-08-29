/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  task,
  series,
  option,
  argv,
  tscTask,
  eslintTask,
} = require('just-scripts');
const libPath = path.resolve(process.cwd(), 'lib');
const srcPath = path.resolve(process.cwd(), 'src');

option('production');
option('clean');

task('eslint', () => {
  return eslintTask();
});
task('ts', () => {
  return tscTask({
    pretty: true,
    noEmit: true,
    ...(argv().production && {
      inlineSources: true,
      sourceRoot: path.relative(libPath, srcPath),
    }),
    target: 'es6',
    module: 'commonjs',
  });
});

task('build', series('eslint', 'ts'));
