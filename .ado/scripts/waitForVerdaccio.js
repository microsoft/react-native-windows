#!/usr/bin/env node
// @ts-check

const http = require('http');

function queryForServerStatus() {

  http.get('http://localhost:4873', res => {
    console.log(`Server status: ${res.statusCode}`);
    if (res.statusCode != 200) {
      setTimeout(queryForServerStatus, 2000);
    }
  }).on('error', err => {
    console.log(err.message);
    setTimeout(queryForServerStatus, 2000);
  });
}

console.log('Waiting for verdaccio instance to respond...');

queryForServerStatus();