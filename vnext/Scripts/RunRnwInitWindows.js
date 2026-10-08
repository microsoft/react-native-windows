// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

'use strict';

const path = require('path');
const {createRequire} = require('module');

async function main() {
  const template = process.argv[2];
  if (!template) {
    throw new Error('A React Native Windows template is required.');
  }

  const projectRequire = createRequire(path.join(process.cwd(), 'package.json'));
  const communityCli = projectRequire('@react-native-community/cli');
  const rnwCli = projectRequire('@react-native-windows/cli');
  const initCommand = rnwCli.commands.find(
    command => command.name === 'init-windows',
  );

  if (!initCommand) {
    throw new Error('The installed React Native Windows CLI has no init-windows command.');
  }

  const config = await communityCli.loadConfigAsync({});
  await initCommand.func([], config, {
    template,
    overwrite: true,
    logging: true,
  });
}

main().catch(error => {
  console.error(error);
  process.exitCode = 1;
});
