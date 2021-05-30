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
WebSocket binary integration test server

This will send each incoming message back, in binary form.

`);

const server = new WebSocket.Server({port: 5557});
server.on('connection', ws => {
  ws.binaryType = "arraybuffer";
  ws.on('message', message => {
    console.log(message);

    ws.send(new Uint8Array([4, 5, 6, 7]).buffer);
  });
});
