'use strict';

const chalk = require('chalk');
const build = require('./utils/build');
const deploy = require('./utils/deploy');

function runWindows(config, args, options) {
  // Fix up options
  options.root = options.root || process.cwd();
  if (options.debug && options.release) {
    console.log(chalk.red('Only one of "debug"/"release" options should be specified'));
    return;
  }

  const slnFile = build.getSolutionFile(options);
  if (!slnFile) {
    console.error(chalk.red('Visual Studio Solution file not found. Maybe run "rnpm windows" first?'));
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
  const buildArch = options.arch ? options.arch : 'x86';

  try {
    build.buildSolution(slnFile, buildType, buildArch);
  } catch (e) {
    console.error(chalk.red(`Build failed with message ${e}. Check your build configuration.`));
    return;
  }

  return deploy.startServerInNewWindow(options)
    .then(() => {
      if (options.device || options.emulator || options.target) {
        return deploy.deployToDevice(options);
      } else {
        return deploy.deployToDesktop(options);
      }
    })
    .catch(e => console.error(chalk.red(`Failed to deploy: ${e.message}`)));
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
 *    root: String - The root of the application
 *    debug: Boolean - Specifies debug build
 *    release: Boolean - Specifies release build
 *    arch: String - The build architecture (x86, x64, ARM, Any CPU)
 *    desktop: Boolean - Deploy to the desktop
 *    emulator: Boolean - Deploy to the emulator
 *    device: Boolean - Deploy to a device
 *    target: String - Device GUID to deploy to
 *    proxy: Boolean - Run using remote JS proxy
 */
module.exports = {
  name: 'run-windows',
  description: 'builds your app and starts it on a connected Windows desktop, emulator or device',
  func: runWindows,
  options: [{
    flags: '--release',
    description: 'Specifies a release build',
  }, {
    flags: '--root [string]',
    description: 'Override the root directory for the windows build which contains the windows folder.',
  }, {
    flags: '--arch [string]',
    description: 'The build architecture (ARM, x86, x64)',
    default: 'x86',
  }, {
    flags: '--emulator',
    description: 'Deploys the app to an emulator',
  }, {
    flags: '--device',
    description: 'Deploys the app to a connected device',
  }, {
    flags: '--target [string]',
    description: 'Deploys the app to the specified GUID for a device.',
  }, {
    flags: '--proxy',
    description: 'Deploys the app in remote debugging mode.',
  }]
};
