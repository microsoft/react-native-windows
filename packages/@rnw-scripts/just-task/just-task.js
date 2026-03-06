/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('node:fs');
const path = require('node:path');
const os = require('node:os');
const {spawnSync} = require('node:child_process');

const {
  argv,
  cleanTask,
  eslintTask,
  logger,
  jestTask,
  option,
  prettierCheckTask,
  prettierTask,
  series,
  task,
  tscTask,
  tscWatchTask,
  parallel,
  addResolvePath,
} = require('just-scripts');

const findUp = require('find-up');

// Allow searching from current directory to be able to resolve peerDependencies
// when running without hoisting
addResolvePath(__dirname);

option('updateSnapshot', {alias: 'u', boolean: true});

task('clean', cleanTask(['lib-commonjs']));

task('ts', tscTask({outDir: 'lib-commonjs'}));

task('jest', jestTask());
task('jest:watch', jestTask({watch: true}));

task('build', series('ts'));

task('rebuild', series('clean', 'build'));

task('depcheck', () => {
  const bin = path.join(
    path.dirname(path.dirname(require.resolve('knip'))),
    'bin',
    'knip.js'
  );

  const result = spawnSync(process.execPath, [bin, '--include', 'unlisted'], {
    cwd: process.cwd(),
    stdio: 'inherit',
  });

  if (result.status !== 0) {
    logger.error("knip found undeclared dependencies (see above)");
    process.exit(result.status ?? 1);
  }
});

task('prettier', prettierCheckTask({ files: path.resolve(process.cwd(), '**', '*.{ts,tsx,js,jsx}'), ignorePath: findUp.sync('.prettierignore', {cwd: __dirname}) }));
task('prettier:fix', prettierTask({ files: path.resolve(process.cwd(), '**', '*.{ts,tsx,js,jsx}'), ignorePath: findUp.sync('.prettierignore', {cwd: __dirname}) }));

task('eslint', eslintTask());
task('eslint:fix', eslintTask({fix: true}));

task('lint', parallel(series('prettier', 'eslint'), 'depcheck'));
task('lint:fix', series('prettier:fix', 'eslint:fix'));

task('watch', tscWatchTask({outDir: 'lib-commonjs'}));

task(
  'unitTest',
  jestTask({
    config: require.resolve('@rnw-scripts/jest-unittest-config'),
    updateSnapshot: argv().updateSnapshot,
  }),
);
task(
  'endToEndTest',
  jestTask({
    config: require.resolve('@rnw-scripts/jest-e2e-config'),
    runInBand: true,
    updateSnapshot: argv().updateSnapshot,
  }),
);

const windowsOnly = JSON.parse(
  fs.readFileSync(path.join(process.cwd(), 'package.json')),
).windowsOnly;
if (os.platform() !== 'win32' && windowsOnly === true) {
  task('test', () =>
    logger.warn('Skipping tests since "package.json" has set "windowsOnly"'),
  );
} else {
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
}
