'use strict';

const chalk = require('chalk');
const build = require('./utils/build');
const deploy = require('./utils/deploy');

function runWpf(config, args, options) {
  // Fix up options
  options.root = options.root || process.cwd();

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
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe'
});
*/

/**
 * Starts the app on a Windows desktop.
 * Options are the following:
 *    release: Boolean - Specifies release build
 *    root: String - The root of the application
 *    arch: String - The build architecture (x86, x64, ARM, Any CPU)
 *    proxy: Boolean - Run using remote JS proxy
 *    verbose: Boolean - Enables logging
 *    no-packager: Boolean - Do not launch packager while building
 */
module.exports = {
  name: 'run-wpf',
  description: 'builds your app and starts it on a Windows desktop!',
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
    command: '--proxy',
    description: 'Deploys the app in remote debugging mode.',
  }, {
    command: '--verbose',
    description: 'Enables logging',
    default: false,
  }, {
    command: '--no-packager',
    description: 'Do not launch packager while building'
  }]
};
