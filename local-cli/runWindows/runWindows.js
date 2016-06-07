'use strict';

const glob = require('glob');
const fs = require('fs');
const path = require('path');
const chalk = require('chalk');
const parseCommandLine = require('./util/parseCommandLine');
const Promise = require('promise');

/**
 * Starts the app on a connected Windows emulator or mobile device.
 */
function runWindows(argv, config) {
  return new Promise((resolve, reject) => {
    runWindowsInternal(argv, config, resolve, reject);
  });
}

function runWindowsInternal(argv, config, resolve, reject) {
  const args = parseCommandLine([{
    command: 'root',
    type: 'string',
    required: false
  }], argv);

  args.root = args.root || '';

  const slnResults = getSolutionFile(args);
  if (slnResults.length === 0) {
    console.log(chalk.red('Visual Studio solution file not found. Maybe run rnpm windows first?'));
    return;
  }

  const slnFile = slnResults[0];
}

function getSolutionFile(args) {
  return glob.sync(path.join(args.root, 'windows/*.sln'));
}

module.exports = runWindows;
