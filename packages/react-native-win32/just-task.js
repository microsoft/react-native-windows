/**
 * Copyright (c) Microsoft Corporation.
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
  logger,
} = require('just-scripts');
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
task('eslint:fix', () => {
  return eslintTask({fix: true});
});
task('copyFlowPlatformOverrides', () => {
  return copyTask(['src/**/*.js'], '.');
});
task('copyPngFiles', () => {
  return copyTask(['src/**/*.png'], '.');
});
task('copyTypescriptTypeDefFiles', () => {
  return copyTask(['src/**/*.d.ts'], '.');
});
task('copyRNLibraries', () => {
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
  const rnSrcFiles = glob.sync('**', {cwd: srcPath});
  
  const rnOutputs = new Set(rnSrcFiles.flatMap(srcFile => {
    const {dir, name} = path.parse(srcFile);
    const baseName = path.format({dir, name});
    return glob.sync(`${baseName}*(.d)+(.js|.jsx|.ts|.tsx)*(.map)`, {absolute: true});
  }));

  logger.info(`Removing ${rnOutputs.size} files`);
  rnOutputs.forEach(fs.unlinkSync);
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
    path.resolve(
      __dirname,
      'dist/win32/dev/RNTester/js/RNTesterApp.win32.bundle',
    ),
  );
});

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'copyRNLibraries',
    'copyFlowPlatformOverrides',
    'copyPngFiles',
    'copyTypescriptTypeDefFiles',
    'ts',
    condition('apiExtractorVerify', () => argv().ci),
  ),
);

task('lint', series('eslint', 'flow-check'));
task('lint:fix', series('eslint:fix'));

task('api', series('apiExtractorUpdate', 'apiDocumenter'));
