/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check
'use strict';

const build = require('./utils/build');
const deploy = require('./utils/deploy');
const {newError, newInfo} = require('./utils/commandWithProgress');

async function runWindows(config, args, options) {
  const verbose = options.logging;
  if (verbose) {
    newInfo('Verbose: ON');
  }

  // Fix up options
  options.root = options.root || process.cwd();

  if (options.build) {
    const slnFile = build.getSolutionFile(options);
    if (!slnFile) {
      newError(
        'Visual Studio Solution file not found. Maybe run "react-native windows" first?',
      );
      return;
    }

    try {
      await build.restoreNuGetPackages(options, slnFile, verbose);
    } catch (e) {
      newError('Failed to restore the NuGet packages: ' + e.toString());
      return;
    }

    // Get build/deploy options
    const buildType = deploy.getBuildConfiguration(options);
    try {
      await build.buildSolution(slnFile, buildType, options.arch, verbose);
    } catch (e) {
      newError(
        `Build failed with message ${e}. Check your build configuration.`,
      );
      return;
    }
  } else {
    newInfo('Build step is skipped');
  }

  await deploy.startServerInNewWindow(options, verbose);

  try {
    if (options.device || options.emulator || options.target) {
      await deploy.deployToDevice(options, verbose);
    } else {
      await deploy.deployToDesktop(options, verbose);
    }
  } catch (e) {
    newError(`Failed to deploy: ${e.message}`);
    return;
  }
}

/*
// Example of running the Windows Command
runWindows({
  root: 'C:\\github\\hack\\myapp',
  debug: true,
  arch: 'x86',
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe'
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
 *    no-packager: Boolean - Do not launch packager while building
 *    bundle: Boolean - Enable Bundle configuration.
 *    no-launch: Boolean - Do not launch the app after deployment
 *    no-build: Boolean - Do not build the solution
 *    force: Boolean - same as Add-AppDevPackage.ps1 Force flag
 */
module.exports = {
  name: 'run-windows',
  description:
    'builds your app and starts it on a connected Windows desktop, emulator or device',
  func: runWindows,
  options: [
    {
      command: '--release',
      description: 'Specifies a release build',
    },
    {
      command: '--root [string]',
      description:
        'Override the root directory for the windows build which contains the windows folder.',
    },
    {
      command: '--arch [string]',
      description: 'The build architecture (ARM, x86, x64)',
      default: 'x86',
    },
    {
      command: '--emulator',
      description: 'Deploys the app to an emulator',
    },
    {
      command: '--device',
      description: 'Deploys the app to a connected device',
    },
    {
      command: '--target [string]',
      description: 'Deploys the app to the specified GUID for a device.',
    },
    {
      command: '--proxy',
      description: 'Deploys the app in remote debugging mode.',
    },
    {
      command: '--logging',
      description: 'Enables logging',
      default: false,
    },
    {
      command: '--no-packager',
      description: 'Do not launch packager while building',
    },
    {
      command: '--bundle',
      description:
        'Enable Bundle configuration and it would be ReleaseBundle/DebugBundle other than Release/Debug',
      default: false,
    },
    {
      command: '--force',
      description: 'same as Add-AppDevPackage.ps1 Force flag',
      default: false,
    },
    {
      command: '--no-launch',
      description: 'Do not launch the app after deployment',
      default: false,
    },
    {
      command: '--no-build',
      description: 'Do not build the solution',
      default: false,
    },
  ],
};
