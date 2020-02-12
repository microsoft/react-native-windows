#!/usr/bin/env node
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */
'use strict';

/* eslint-env node */

console.log(`\
Test server for WebSocketExample

This will set a cookie named "wstest" on the response of any incoming request.
`);

const connect = require('connect');
const http = require('http');

const app = connect();

app.use(function(req, res) {
  console.log('received request');
  res.setHeader('Set-Cookie', ['wstest=OK; Path=/']);
  res.end('Cookie has been set!\n');
});

http.createServer(app).listen(5556);
