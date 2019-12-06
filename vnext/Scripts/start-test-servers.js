/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const {spawn} = require('child_process');
const {dirname} = require('path');

const serverEntries = {
  metroServer: null,
  websocketServer: null,
};

function launchMetroServer() {
  return new Promise((resolve, reject) => {
    console.log('Launching metro server...');
    serverEntries.metroServer = spawn(
      'node',
      [
        require.resolve('react-native/local-cli/cli.js'),
        'start',
        '--config',
        './metro.config.js',
      ],
      {cwd: dirname(__dirname)},
    );

    serverEntries.metroServer.stdout.on('data', chunk => {
      console.log(chunk.toString());
      if (chunk.toString().indexOf('done') >= 0) {
        resolve();
      }
    });
    serverEntries.metroServer.on('error', err => {
      console.error('Metro process failed!' + err.toString());
      reject(err);
    });

    serverEntries.metroServer.on('exit', code => {
      if (code !== 0) {
        const err = `Metro process failed with error code: ${code}`;
        console.error(err);
        reject(err);
      }
    });
  });
}

function launchWebsocketServer() {
  return new Promise((resolve, reject) => {
    console.log('Launching test websocket server...');
    serverEntries.websocketServer = spawn(
      'node',
      [
        require.resolve(
          'react-native/IntegrationTests/websocket_integration_test_server.js',
        ),
      ],
      {cwd: dirname(__dirname)},
    );

    serverEntries.websocketServer.stdout.on('data', chunk => {
      console.log(chunk.toString());
      if (chunk.toString().indexOf('WebSocket integration test server') >= 0) {
        resolve();
      }
    });
    serverEntries.websocketServer.on('error', err => {
      reject(err);
    });

    serverEntries.websocketServer.on('exit', code => {
      if (code !== 0) {
        const err = `Websocket test server process failed with error code: ${code}`;
        console.error(err);
        reject(err);
      }
    });
  });
}

launchMetroServer().then(launchWebsocketServer);
