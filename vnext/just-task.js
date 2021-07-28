/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  cleanTask,
  task,
  series,
  condition,
  option,
  argv,
  tscTask,
} = require('just-scripts');

// Use the shared base configuration
require('@rnw-scripts/just-task');
require('@rnw-scripts/just-task/react-native-tasks');

const {execSync} = require('child_process');
const fs = require('fs');

option('production');
option('clean');

task(
  'codegen',
  series(cleanTask({paths: ['./codegen']}), () => {
    execSync(
      'npx --no-install @react-native-windows/codegen --files Libraries/**/*Native*.js --namespace Microsoft::ReactNativeSpecs --libraryName rnwcore',
    );
    execSync(
      'npx --no-install @react-native-windows/codegen --files Libraries/**/*NativeComponent.js --namespace Microsoft::ReactNativeSpecs --libraryName rnwcore',
    );
    fs.writeFileSync(
      'codegen/.clang-format',
      'DisableFormat: true\nSortIncludes: false',
    );
  }),
);

task('copyReadmeAndLicenseFromRoot', () => {
  fs.copyFileSync(
    path.resolve(__dirname, '../README.md'),
    path.resolve(__dirname, 'README.md'),
  );
  fs.copyFileSync(
    path.resolve(__dirname, '../LICENSE'),
    path.resolve(__dirname, 'LICENSE'),
  );
});

task('compileTsPlatformOverrides', tscTask());

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'copyRNLibraries',
    'copyReadmeAndLicenseFromRoot',
    'compileTsPlatformOverrides',
    'codegen',
  ),
);

task('clean', series('cleanRNLibraries'));

task('lint', series('eslint', 'flow-check'));
