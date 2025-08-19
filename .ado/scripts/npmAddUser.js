#!/usr/bin/env node
// @ts-check

const child_process = require("child_process");
const fs = require("fs");
const path = require("path");
const os = require("os");

const username = process.argv[2];
const password = process.argv[3];
const email = process.argv[4];
const registry = process.argv[5];

if (!username) {
  console.error("Please specify username");
  process.exit(1);
}

if (!password) {
  console.error("Please specify password");
  process.exit(1);
}

if (!email) {
  console.error("Please specify email");
  process.exit(1);
}

const registryUrl = registry || "http://localhost:4873";

// First set the registry
console.log(`Setting npm registry to ${registryUrl}`);
const setRegistry = child_process.spawnSync('npm', ['config', 'set', 'registry', registryUrl], {
  stdio: 'inherit',
  shell: true
});

if (setRegistry.status !== 0) {
  console.error('Failed to set registry');
  process.exit(1);
}

// Create auth token for verdaccio
const authString = Buffer.from(`${username}:${password}`).toString('base64');
const registryPath = registryUrl.replace(/^https?:/, '');

// Set auth in npm config
console.log('Setting authentication...');
const setAuth = child_process.spawnSync('npm', ['config', 'set', `${registryPath}/:_auth`, authString], {
  stdio: 'inherit',
  shell: true
});

if (setAuth.status !== 0) {
  console.error('Failed to set auth');
  process.exit(1);
}

// Set email
const setEmail = child_process.spawnSync('npm', ['config', 'set', 'email', email], {
  stdio: 'inherit',
  shell: true
});

// Set always-auth
child_process.spawnSync('npm', ['config', 'set', 'always-auth', 'true'], {
  stdio: 'inherit',
  shell: true
});

// Verify authentication
console.log('Verifying authentication...');
const whoami = child_process.spawnSync('npm', ['whoami', '--registry', registryUrl], {
  encoding: 'utf8',
  shell: true
});

if (whoami.status === 0 && whoami.stdout.trim()) {
  console.log(`Logged in as ${whoami.stdout.trim()} on ${registryUrl}`);
  process.exit(0);
} else {
  console.error('Authentication verification failed');
  if (whoami.stderr) console.error('Error:', whoami.stderr);
  process.exit(1);
}
