'use strict';

const child_process = require('child_process');
const spawn = child_process.spawn, execSync = child_process.execSync;
const fs = require('fs');
const http = require('http');
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const parse = require('xml-parser');

function deployToDesktop(options) {

  const args = ["remoteDebugging", options.proxy ? 'true' : 'false'];
  const appName = process.cwd().split(path.sep).pop();
  const launchAppScript = path.join(`./wpf/${appName}/bin/x86/Debug/${appName}`);

  console.log(chalk.green('Starting the app'));
  return Promise.resolve(spawn('cmd.exe', ['/C', 'start', launchAppScript], args));

}

function startServerInNewWindow(options) {
  return new Promise(resolve => {
    http.get('http://localhost:8081/status', res => {
      if (res.statusCode === 200) {
        console.log(chalk.green('React-Native Server already started'));
      } else {
        console.log(chalk.red('React-Native Server not responding'));
      }
      resolve();
    }).on('error', () => resolve(launchServer(options)));
  });
}

function launchServer(options) {
  console.log(chalk.green('Starting the React-Native Server'));
  const launchPackagerScript = path.join('node_modules/react-native/packager/launchPackager.bat');
  const opts = {
    cwd: options.root,
    detached: true,
    stdio: 'ignore'
  };

  return Promise.resolve(spawn('cmd.exe', ['/C', 'start', launchPackagerScript], opts));
}

module.exports = {
  deployToDesktop,
  startServerInNewWindow
};
