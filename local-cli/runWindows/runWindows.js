'use strict';

const chalk = require('chalk');
const automatedVS = require('../automated-vs/index');
const UWPSolution = automatedVS.UWPSolution;
const buildAndDeployUtils = require('../utils/buildAndDeployUtils');

const minWinSDK = '10.0.14393.0';

function runWindows(config, args, options) {

  options.root = options.root || process.cwd();

  const slnFile = buildAndDeployUtils.getSolutionFile(options, 'windows');
  if (!slnFile) {
    console.error(chalk.red('Visual Studio Solution file not found. Make sure your root directory contains the wpf folder.'));
    return;
  }

  const uwpSol = new UWPSolution(slnFile, options.root, minWinSDK);

  try {
    uwpSol.build(options);
  } catch (e) {
    console.error(chalk.red(`Build failed with message ${e}. Check your build configuration.`));
    return;
  }

  return buildAndDeployUtils.startServerInNewWindow(options).then(() => uwpSol.deploy(options, `windows/${uwpSol.name}`));
}

/*
// Example of running the Windows Command
runWindows({
  root: 'C:\\github\\hack\\myapp',
  debug: true,
  arch: 'x86',
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe',
  desktop: true
});
*/

/**
 * Starts the app on a connected Windows emulator or mobile device.
 * Options are the following:
 *    release: Boolean - Specifies release build
 *    root: String - The root of the application
 *    arch: String - The build architecture (x86, x64, ARM, Any CPU)
 *    emulator: Boolean - Deploy to the emulator
 *    device: Boolean - Deploy to a device
 *    target: String - Device GUID to deploy to
 *    proxy: Boolean - Run using remote JS proxy
 *    verbose: Boolean - Enables logging
 */
module.exports = {
  name: 'run-windows',
  description: 'builds your app and starts it on a connected Windows desktop, emulator or device',
  func: runWindows,
  options: [{
    command: '--release',
    description: 'Specifies a release build',
  }, {
    command: '--root [string]',
    description: 'Override the root directory for the windows build which contains the windows folder.',
  }, {
    command: '--arch [string]',
    description: 'The build architecture (ARM, x86, x64)',
    default: 'x86',
  }, {
    command: '--emulator',
    description: 'Deploys the app to an emulator',
  }, {
    command: '--device',
    description: 'Deploys the app to a connected device',
  }, {
    command: '--target [string]',
    description: 'Deploys the app to the specified GUID for a device.',
  }, {
    command: '--proxy',
    description: 'Deploys the app in remote debugging mode.',
  }, {
    command: '--verbose',
    description: 'Enables logging',
    default: false,
  }]
};
