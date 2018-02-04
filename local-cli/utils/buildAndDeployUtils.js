'use strict';

const path = require('path');
const glob = require('glob');
const child_process = require('child_process');
const spawn = child_process.spawn;
const http = require('http');
const chalk = require('chalk');

function getSolutionFile(options, platform) {
  return glob.sync(path.join(options.root, platform + '/*.sln'))[0];
}

function startServerInNewWindow(options) {
  return new Promise((resolve)=> {
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
  const launchPackagerScript = path.join('node_modules/react-native/scripts/launchPackager.bat');
  const opts = {
    cwd: options.root,
    detached: true,
    stdio: options.verbose ? 'inherit' : 'ignore'
  };

  return Promise.resolve(spawn('cmd.exe', ['/C', 'start', launchPackagerScript], opts));
}

module.exports = {
  getSolutionFile,
  startServerInNewWindow
};
