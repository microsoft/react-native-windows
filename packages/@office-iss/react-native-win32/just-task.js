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
  series,
  condition,
  option,
  argv,
  tscTask,
} = require('just-scripts');

// Use the shared base configuration
require('@rnw-scripts/just-task');
require('@rnw-scripts/just-task/react-native-tasks');

const hermesUtils = require('react-native/scripts/hermes/hermes-utils');

option('production');
option('clean');

task('cgmanifests', () => {
  // Get hermes version used
  const hermesTag = hermesUtils.readHermesTag();
  const hermesTagSHA = hermesUtils.getHermesTagSHA(hermesTag);

  // Construct CG manifest object
  const cgmanifest = {
    Registrations: [
      {
        Component: {
          Type: 'git',
          Git: {
            RepositoryUrl: 'https://github.com/facebook/hermes',
            CommitHash: hermesTagSHA,
          },
        },
        DevelopmentDependency: false,
      },
    ],
  };

  // Write CG manifest object to JSON
  fs.writeFileSync(
    path.resolve(__dirname, 'cgmanifest.json'),
    JSON.stringify(cgmanifest, null, 2),
  );
});

task('ts', tscTask());

task('prepareBundle', () => {
  fs.mkdirSync(
    path.resolve(__dirname, 'dist/win32/dev/js/RNTesterApp.win32.bundle'),
    {recursive: true},
  );
});

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'copyRNLibraries',
    'ts',
    'cgmanifests',
  ),
);

task('clean', series('cleanRNLibraries'));

task('lint', series('eslint', 'flow-check'));
