'use strict';

const child_process = require('child_process');
const spawn = child_process.spawn;
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

function startServerInNewWindow(options, reactNativePath) {
  return new Promise(resolve => {
    if (options.packager) {
      http.get(`http://localhost:${options.port}/status`, res => {
        if (res.statusCode === 200) {
          console.log(chalk.green('React-Native Server already started'));
        } else {
          console.log(chalk.red('React-Native Server not responding'));
        }
        resolve();
      }).on('error', () => resolve(launchServer(options.port, reactNativePath)));
    } else {
      resolve();
    }
  });
}

function launchServer(port, reactNativePath)
{
  /**
   * Set up OS-specific filenames and commands
   */
  const scriptFile = 'launchPackager.bat';
  const packagerEnvFilename = '.packager.bat';
  const portExportContent = `set RCT_METRO_PORT=${port}`;

  /**
   * Quick & temporary fix for packager crashing on Windows due to using removed --projectRoot flag
   * in script. So we just replace the contents of the script with the fixed version. This should be
   * removed when PR #25517 on RN Repo gets approved and a new RN version is released.
   */
  const launchPackagerScriptContent = `:: Copyright (c) Facebook, Inc. and its affiliates.
  ::
  :: This source code is licensed under the MIT license found in the
  :: LICENSE file in the root directory of this source tree.
  @echo off
  title Metro Bundler
  call .packager.bat
  cd ../../../
  node "%~dp0..\\cli.js" start
  pause
  exit`;

  /**
   * Set up the `.packager.bat` file to ensure the packager starts on the right port.
   */
  const launchPackagerScript = path.join(
    reactNativePath,
    `scripts/${scriptFile}`,
  );

  /**
   * Set up the `launchpackager.bat` file.
   * It lives next to `.packager.bat`
   */
  const scriptsDir = path.dirname(launchPackagerScript);
  const packagerEnvFile = path.join(scriptsDir, packagerEnvFilename);
  const procConfig = {cwd: scriptsDir};

  /**
   * Ensure we overwrite file by passing the `w` flag
   */
  fs.writeFileSync(packagerEnvFile, portExportContent, {
    encoding: 'utf8',
    flag: 'w',
  });

  procConfig.detached = true;
  procConfig.stdio = 'ignore';
  //Temporary fix for #484. See comment on line 254
  fs.writeFileSync(launchPackagerScript, launchPackagerScriptContent, {
    encoding: 'utf8',
    flag: 'w',
  });

  return Promise.resolve(spawn('cmd.exe', ['/C', launchPackagerScript], procConfig));
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
