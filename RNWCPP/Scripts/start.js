// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

const { execSync } = require('child_process');
const { realpathSync } = require('fs');
const { resolve } = require('path');

// sdx-platform needs to specify the config file since rn has issues with symlinked node_modules
execSync(
  `node node_modules/@microsoft/react-native/local-cli/cli.js start --config ${realpathSync(
    resolve(__dirname, '../rn-cli.config.js')
  )}`,
  { stdio: 'inherit' }
);
