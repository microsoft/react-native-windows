// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// @ts-check

const path = require('path');
const { task, series, condition, option, argv } = require('just-task');
const { tscTask, tslintTask, cleanTask } = require('just-scripts');
const libPath = path.resolve(process.cwd(), 'lib');
const srcPath = path.resolve(process.cwd(), 'src');

if (require('fs').existsSync(path.resolve(__dirname, '../../../scripts/just-task.js'))) {
  require('../../../scripts/just-task')
} else {

  option('production');
  option('clean');

  task('tslint', () => {
    return tslintTask();
  });
  task('ts', () => {
    return tscTask({
      pretty: true,
      ...(argv().production && { inlineSources: true, sourceRoot: path.relative(libPath, srcPath) }),
      target: 'es5',
      outDir: 'lib',
      module: 'commonjs',
    });
  });
  task('clean', () => {
    return cleanTask(['lib', 'temp', 'dist', 'coverage'].map(p => path.join(process.cwd(), p)));
  });

  task(
    'build',
    series(
      condition('clean', () => argv().clean),
      'tslint',
      'ts'
    )
  );}
