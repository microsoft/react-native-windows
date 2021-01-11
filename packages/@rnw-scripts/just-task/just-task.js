/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');
const path = require('path');

const {
  cleanTask,
  eslintTask,
  logger,
  jestTask,
  series,
  task,
  tscTask,
  tscWatchTask,
} = require('just-scripts');

task('clean', cleanTask(['lib-commonjs']));

task('ts', tscTask({outDir: 'lib-commonjs'}));

task('jest', jestTask());
task('jest:watch', jestTask({watch: true}));

task('build', series('ts'));

task('rebuild', series('clean', 'build'));

task('eslint', eslintTask());
task('eslint:fix', eslintTask({fix: true}));

task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));

task('watch', tscWatchTask({outDir: 'lib-commonjs'}));

task(
  'unitTest',
  jestTask({
    config: require.resolve('@rnw-scripts/jest-unittest-config'),
    _: ['--verbose'],
  }),
);
task(
  'endToEndTest',
  jestTask({
    config: require.resolve('@rnw-scripts/jest-e2e-config'),
    runInBand: true,
    _: ['--verbose'],
  }),
);

const hasE2eTests = fs.existsSync(path.join(process.cwd(), 'src', 'e2etest'));
const hasUnitTests = fs.existsSync(path.join(process.cwd(), 'src', 'test'));

if (hasE2eTests && hasUnitTests) {
  task('test', series('unitTest', 'endToEndTest'));
} else if (hasE2eTests) {
  task('test', 'endToEndTest');
} else if (hasUnitTests) {
  task('test', 'unitTest');
} else {
  task('test', () => logger.info('No tests found'));
}
