'use-strict';

const chalk = require('chalk');
const build = require('./utils/build');
const deploy = require('./utils/deploy');
const CONSTANTS = require('./constants');

function runRNWApp(config, args, options, wpfOrWindows) {
    // Fix up options
  options.root = options.root || process.cwd();
  if (options.debug && options.release) {
    console.log(chalk.red('Only one of "debug"/"release" options should be specified'));
    return;
  }

  const slnFile = build.getSolutionFile(options, wpfOrWindows);
  if (!slnFile) {
    wpfWindowsStr = wpfOrWindows === CONSTANTS.windows? 'windows' : 'wpf';
    console.error(chalk.red('Visual Studio Solution file not found. Maybe run "react-native ' + wpfWindowsStr +  '" first?'));
    return;
  }

  try {
    build.restoreNuGetPackages(options, slnFile, options.verbose, wpfOrWindows);
  } catch (e) {
    console.error(chalk.red('Failed to restore the NuGet packages'));
    return;
  }

  // Get build/deploy options
  const buildType = options.release ? 'Release' : 'Debug';

  try {
    build.buildSolution(slnFile, buildType, options.arch, options.verbose, wpfOrWindows);
  } catch (e) {
    console.error(chalk.red(`Build failed with message ${e}. Check your build configuration.`));
    return;
  }



  return deploy.startServerInNewWindow(options)
    .then(() => {
      if (wpfOrWindows === CONSTANTS.windows) {
        if (options.device || options.emulator || options.target) {
            return deploy.deployToDevice(options);
          } else {
            return deploy.deployWindowsToDesktop(options);
          }
      } else {
        return deploy.deployWpfToDesktop(options);
      }
    })
    .catch(e => console.error(chalk.red(`Failed to deploy: ${e.message}`)));
}

module.exports = { runRNWApp };