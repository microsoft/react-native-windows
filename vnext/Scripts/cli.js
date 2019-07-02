/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

const {execSync} = require('child_process');
const {realpathSync} = require('fs');
const {resolve} = require('path');

const args = process.argv.slice(2).join(' ');

// sdx-platform needs to specify the config file since rn has issues with symlinked node_modules
execSync(
  `node node_modules/react-native/local-cli/cli.js ${args} --config ${realpathSync(
    resolve(__dirname, '../rn-cli.config.js'),
  )}`,
  {stdio: 'inherit'},
);
