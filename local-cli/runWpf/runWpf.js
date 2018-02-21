'use strict';

const chalk = require('chalk');
const build = require('./utils/build');
const deploy = require('./utils/deploy');

function runWpf(config, args, options) {
  // Fix up options
  options.root = options.root || process.cwd();
  if (options.debug && options.release) {
    console.log(chalk.red('Only one of "debug"/"release" options should be specified'));
    return;
  }

  const slnFile = build.getSolutionFile(options);
  if (!slnFile) {
    console.error(chalk.red('Visual Studio Solution file not found. Maybe run "react-native wpf" first?'));
    return;
  }

  try {
    build.restoreNuGetPackages(options, slnFile);
  } catch (e) {
    console.error(chalk.red('Failed to restore the NuGet packages'));
    return;
  }

  // Get build/deploy options
  const buildType = options.release ? 'Release' : 'Debug';

  try {
    build.buildSolution(slnFile, buildType, options.arch, options.verbose);
  } catch (e) {
    console.error(chalk.red(`Build failed with message ${e}. Check your build configuration.`));
    return;
  }

  return deploy.startServerInNewWindow(options)
    .then(() => {
      return deploy.deployToDesktop(options);
    })
    .catch(e => console.error(chalk.red(`Failed to deploy: ${e.message}`)));
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
 *    root: String - The root of the application
 *    debug: Boolean - Specifies debug build
 *    release: Boolean - Specifies release build
 *    arch: String - The build architecture (x86, x64, ARM, Any CPU)
 *    desktop: Boolean - Deploy to the desktop
 *    emulator: Boolean - Deploy to the emulator
 *    device: Boolean - Deploy to a device
 *    target: String - Device GUID to deploy to
 *    proxy: Boolean - Run using remote JS proxy
 *    no-packager: Boolean - Do not launch packager while building
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
  }, {
    command: '--no-packager',
    description: 'Do not launch packager while building'
  }]
};
