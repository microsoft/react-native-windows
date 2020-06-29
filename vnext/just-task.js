/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  task,
  cleanTask,
  copyTask,
  series,
  condition,
  option,
  argv,
  tscTask,
  eslintTask,
  apiExtractorVerifyTask,
  apiExtractorUpdateTask,
  parallel,
  logger
} = require('just-scripts');
const {execSync} = require('child_process');
const fs = require('fs');
const glob = require('glob');
const srcPath = path.resolve(process.cwd(), 'src');

option('production');
option('clean');
option('ci');

task('apiExtractorVerify', apiExtractorVerifyTask());
task('apiExtractorUpdate', apiExtractorUpdateTask());

task('apiDocumenter', () => {
  require('child_process').execSync(
    'npx @microsoft/api-documenter markdown -i temp -o ../docs/api',
    {stdio: 'inherit'},
  );
});

task('codegen', () => {
  execSync(
    'npx react-native-windows-codegen --files Libraries/**/Native*.js --namespace Microsoft::ReactNativeSpecs',
  );
});

task('flow-check', () => {
  require('child_process').execSync('npx flow check', {stdio: 'inherit'});
});

task('eslint', eslintTask());
task('eslint:fix', eslintTask({fix: true}));

task('copyFlowPlatformOverrides', copyTask(['src/**/*.js'], '.'));
task('copyRNLibraries', () => {
  require('./Scripts/copyRNLibraries').copyRNLibraries(__dirname);
});

task('copyReadmeFromRoot', () => {
  fs.copyFileSync(
    path.resolve(__dirname, '../README.md'),
    path.resolve(__dirname, 'README.md'),
  );
});

task('compileTsPlatformOverrides',  tscTask({
    project: path.join(srcPath, 'tsconfig.json'),
    pretty: true,
    ...(argv().production && {
      inlineSources: true,
    }),
    target: 'es5',
    module: 'commonjs',
}));

task('compileLocalCli', tscTask({
    project: path.join('./local-cli/tsconfig.json')
}));

task('cleanRNLibraries', () => {
  const rnSrcFiles = glob.sync('**', {cwd: srcPath});
  
  const rnOutputs = new Set(rnSrcFiles.flatMap(srcFile => {
    const {dir, name} = path.parse(srcFile);
    const baseName = path.format({dir, name});
    return glob.sync(`${baseName}*(.d)+(.js|.jsx|.ts|.tsx)*(.map)`, {absolute: true});
  }));

  logger.info(`Removing ${rnOutputs.size} files`);
  rnOutputs.forEach(fs.unlinkSync);
});

task('cleanLocalCli', cleanTask(['local-cli/lib-commonjs']));

task(
  'build',
  parallel(
    series(
      condition('clean', () => argv().clean),
      'copyRNLibraries',
      'copyFlowPlatformOverrides',
      'copyReadmeFromRoot',
      'compileTsPlatformOverrides',
      'codegen',
      condition('apiExtractorVerify', () => argv().ci),
    ),
    'compileLocalCli',
  )
);

task('clean', series('cleanRNLibraries', 'cleanLocalCli'));

task('lint', series('eslint', 'flow-check'));
task('lint:fix', series('eslint:fix'));

task('api', series('apiExtractorUpdate', 'apiDocumenter'));
