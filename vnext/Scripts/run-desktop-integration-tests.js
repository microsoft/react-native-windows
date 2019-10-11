/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

const {
  runBuildCommand,
} = require('../../../packages/sdx-build-tools/lib/utils/run-build-command');

const path = require('path');
const fs = require('fs');
const {spawn} = require('child_process');

if (!process.env.VCINSTALLDIR) {
  throw new Error('Cannot find Visual Studio.');
}

const vstestPath = path.join(
  process.env.VCINSTALLDIR,
  'Common7/IDE/CommonExtensions/Microsoft/TestWindow/vstest.console.exe',
);
if (!fs.existsSync(vstestPath)) {
  throw new Error('Unable to find vstest.console.exe');
}

console.log(JSON.stringify(process.argv));

let dllPath = process.env.RNDesktopUTDLL;

if (!dllPath && process.argv.length !== 3) {
  throw new Error('Expected arg for test dll');
}

if (process.argv.length === 3) {
  dllPath = process.argv[2];
}

let BuildPlatform = process.env.BuildPlatform;
if (!BuildPlatform) {
  BuildPlatform = 'x64';
}

let BuildConfiguration = process.env.BuildConfiguration;
if (!BuildConfiguration) {
  BuildConfiguration = 'Debug';
}

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
        '../node_modules/react-native/local-cli/cli.js',
        'start',
        '--config',
        '../../../react-native-win/metro.config.js',
      ],
      {cwd: __dirname},
    );

    serverEntries.metroServer.stdout.on('data', chunk => {
      console.log(chunk.toString());
      if (chunk.toString().indexOf('Metro Bundler ready') >= 0) {
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
        '../node_modules/react-native/IntegrationTests/websocket_integration_test_server.js',
      ],
      {cwd: __dirname},
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

launchMetroServer()
  .then(launchWebsocketServer)
  .then(() => {
    console.log('Running IntegrationTests');
    runBuildCommand(vstestPath, [
      dllPath,
      '/InIsolation',
      `/Platform:${BuildPlatform}`,
    ]);
    console.log('Killing test servers');
    serverEntries.metroServer.kill();
    serverEntries.websocketServer.kill();
  });
