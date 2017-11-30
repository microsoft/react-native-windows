'use strict';

const chalk = require('chalk');
const automatedVS = require('../automated-vs/index');
const WPFProject = automatedVS.WPFProject;
const buildAndDeployUtils = require('../utils/buildAndDeployUtils');

const minWinSDK = '8.1.0.0';

function runWpf(config, args, options) {

  options.root = options.root || process.cwd();

  const slnFile = buildAndDeployUtils.getSolutionFile(options, 'wpf');
  if (!slnFile) {
    console.error(chalk.red('Visual Studio Solution file not found. Make sure your root directory contains the wpf folder.'));
    return;
  }

  const wpfProj = new WPFProject(slnFile, options.root, null, minWinSDK);

  try {
    wpfProj.build(options);
  } catch (e) {
    console.error(chalk.red(`Build failed with message ${e}. Check your build configuration.`));
    return;
  }


  return buildAndDeployUtils.startServerInNewWindow(options).then(() => wpfProj.deploy(options, `wpf/${wpfProj.name}`));
}

/*
// Example of running the WPF Command
runWpf({
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
 *    verbose: Boolean - Enables logging
 */
module.exports = {
  name: 'run-wpf',
  description: 'builds your app and starts it on a connected Windows desktop, emulator or device!',
  func: runWpf,
  options: [{
    command: '--release',
    description: 'Specifies a release build',
  }, {
    command: '--root [string]',
    description: 'Override the root directory for the windows build which contains the wpf folder.',
  }, {
    command: '--arch [string]',
    description: 'The build architecture (ARM, x86, x64)',
    default: 'x86',
  }, {
    command: '--verbose',
    description: 'Enables logging',
    default: false,
  }]
};
