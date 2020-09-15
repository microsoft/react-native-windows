#!/usr/bin/env node
/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

'use strict';

/* eslint-env node */

const WebSocket = require('ws');

console.log(`\
WebSocket integration test server

This will send each incoming message back, with the string '_response' appended.
An incoming message of 'exit' will shut down the server.

`);

const server = new WebSocket.Server({port: 5555});
server.on('connection', ws => {
  ws.on('message', message => {
    // console.log('Received message:', message);
    // if (message === 'exit') {
    //   console.log('WebSocket integration test server exit');
    //   process.exit(0);
    // }
    // ws.send(message + '_response');

    console.log(message);

    ws.binaryType = "arraybuffer";
    ws.send(new Uint8Array([4, 5, 6, 7]).buffer);
  });

  //ws.send('hello');
});


/*
var ws = new WebSocket('ws://localhost:5555');
ws.onmessage = (msg) => { console.log(msg.data); bl = msg.data };
setTimeout(() => { ws.send(new Uint8Array([1, 2, 3]).buffer) }, 500)
*/
