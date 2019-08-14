/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

const {execSync} = require('child_process');
const path = require('path');
const args = process.argv.slice(2).join(' ');

const rncliLocation = path.resolve(
  require.resolve('react-native/package.json'),
  '../local-cli/cli.js',
);

console.warn('You should be able to use the react-native cli directly now...');
console.warn(
  'Stack of callers printed here for diagnosis of who is using the Scripts/cli.js wrapper',
);
console.warn(new Error().stack);

execSync(`node ${rncliLocation} ${args}`, {
  stdio: 'inherit',
});
