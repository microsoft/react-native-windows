'use strict';

const child_process = require('child_process');
const spawn = child_process.spawn;
const http = require('http');
const path = require('path');
const chalk = require('chalk');

function deployToDesktop(options) {
  const appName = process.cwd().split(path.sep).pop();
  const launchAppScript = path.join(`./wpf/${appName}/bin/x86/Debug/${appName}`);

  console.log(chalk.green('Starting the app'));
  const spawnOptions = options.verbose ? { stdio: 'inherit' } : {};
  return Promise.resolve(spawn('cmd.exe', ['/C', 'start', launchAppScript,
    'remoteDebugging', options.proxy ? 'true' : 'false'], spawnOptions));

}

function startServerInNewWindow(options) {
  return new Promise(resolve => {
    if (options.packager) {
      http.get('http://localhost:8081/status', res => {
        if (res.statusCode === 200) {
          console.log(chalk.green('React-Native Server already started'));
        } else {
          console.log(chalk.red('React-Native Server not responding'));
        }
        resolve();
      }).on('error', () => resolve(launchServer(options)));
    } else {
      resolve();
    }
  });
}

function launchServer(options) {
  console.log(chalk.green('Starting the React-Native Server'));
  const launchPackagerScript = path.join('node_modules/react-native/scripts/launchPackager.bat');
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
