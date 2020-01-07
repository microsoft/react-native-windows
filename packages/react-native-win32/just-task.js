/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const fs = require('fs');
const {
  task,
  copyTask,
  series,
  condition,
  option,
  argv,
  tscTask,
  eslintTask,
  apiExtractorVerifyTask,
  apiExtractorUpdateTask,
  cleanTask,
} = require('just-scripts');
const libPath = path.resolve(process.cwd(), 'lib');
const srcPath = path.resolve(process.cwd(), 'src');

option('production');
option('clean');
option('ci');

task('apiExtractorVerify', apiExtractorVerifyTask());
task('apiExtractorUpdate', apiExtractorUpdateTask());

task('apiDocumenter', () => {
  require('child_process').execSync(
    'npx @microsoft/api-documenter markdown -i temp -o docs/api',
    {stdio: 'inherit'},
  );
});

task('eslint', () => {
  return eslintTask();
});
task('copyFlowFiles', () => {
  return copyTask(['src/**/*.js'], '.');
});
task('copyPngFiles', () => {
  return copyTask(['src/**/*.png'], '.');
});
task('initRNLibraries', () => {
  require('../../vnext/scripts/copyRNLibraries').copyRNLibraries(__dirname);
});

task('flow-check', () => {
  require('child_process').execSync('npx flow check', {stdio: 'inherit'});
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
    ['dist', 'flow', 'flow-typed', 'jest', 'Libraries', 'RNTester', 'lib'].map(
      p => path.join(process.cwd(), p),
    ),
  );
});

function ensureDirectoryExists(filePath) {
  const dir = path.dirname(filePath);
  if (!fs.existsSync(dir)) {
    ensureDirectoryExists(dir);
    fs.mkdirSync(dir);
  }
}

task('prepareBundle', () => {
  ensureDirectoryExists(
    path.resolve(__dirname, 'dist/win32/dev/index.win32.bundle'),
  );
});

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'initRNLibraries',
    'copyFlowFiles',
    'copyPngFiles',
    'ts',
    'flow-check',
    condition('apiExtractorVerify', () => argv().ci),
  ),
);

task('lint', series('eslint', 'flow-check'));

task('api', series('apiExtractorUpdate', 'apiDocumenter'));
