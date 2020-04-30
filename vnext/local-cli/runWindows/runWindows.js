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
const info = require('./utils/info');
const msbuildtools = require('./utils/msbuildtools');
const autolink = require('./utils/autolink');
const chalk = require('chalk');

function ExitProcessWithError(loggingWasEnabled) {
  if (!loggingWasEnabled) {
    console.log(
      `Re-run the command with ${chalk.bold('--logging')} for more information`,
    );
  }
  process.exit(1);
}

async function runWindows(config, args, options) {
  const verbose = options.logging;

  if (verbose) {
    newInfo('Verbose: ON');
  }

  if (options.info) {
    try {
      const output = await info.getEnvironmentInfo();
      console.log(output.trimEnd());
      console.log('  Installed UWP SDKs:');
      const sdks = msbuildtools.getAllAvailableUAPVersions();
      if (sdks) {
        sdks.forEach(version => console.log('    ' + version));
      }
      return;
    } catch (e) {
      newError('Unable to print environment info.\n' + e.toString());
      ExitProcessWithError(options.logging);
    }
  }

  // Fix up options
  options.root = options.root || process.cwd();
  const slnFile = options.sln || build.getSolutionFile(options);

  if (options.autolink) {
    autolink.updateAutoLink(verbose);
  }
  if (options.build) {
    if (!slnFile) {
      newError(
        'Visual Studio Solution file not found. Maybe run "react-native windows" first?',
      );
      ExitProcessWithError(options.logging);
    }

    try {
      await build.restoreNuGetPackages(options, slnFile, verbose);
    } catch (e) {
      newError('Failed to restore the NuGet packages: ' + e.toString());
      ExitProcessWithError(options.logging);
    }

    // Get build/deploy options
    const buildType = deploy.getBuildConfiguration(options);
    const msBuildProps = build.parseMsBuildProps(options);

    try {
      await build.buildSolution(
        slnFile,
        buildType,
        options.arch,
        msBuildProps,
        verbose,
      );
    } catch (e) {
      newError(
        `Build failed with message ${
          e.message
        }. Check your build configuration.`,
      );
      if (e.logfile) {
        console.log('See', chalk.bold(e.logfile));
      }
      ExitProcessWithError(options.logging);
    }
  } else {
    newInfo('Build step is skipped');
  }

  await deploy.startServerInNewWindow(options, verbose);

  if (options.deploy) {
    try {
      if (options.device || options.emulator || options.target) {
        await deploy.deployToDevice(options, verbose);
      } else {
        await deploy.deployToDesktop(options, verbose, slnFile);
      }
    } catch (e) {
      newError(`Failed to deploy${e ? `: ${e.message}` : ''}`);
      ExitProcessWithError(options.logging);
    }
  } else {
    newInfo('Deploy step is skipped');
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
 *    no-deploy: Boolean - Do not deploy the app
 *    sln: String - Solution file to build
 *    msBuildProps: String - Comma separated props to pass to msbuild, eg: prop1=value1,prop2=value2
 *    direct-debugging: Number - Enable direct debugging on specified port
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
      description: 'The build architecture (ARM, ARM64, x86, x64)',
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
      command: '--no-launch',
      description: 'Do not launch the app after deployment',
      default: false,
    },
    {
      command: '--no-build',
      description: 'Do not build the solution',
      default: false,
    },
    {
      command: '--no-deploy',
      description: 'Do not deploy the app',
      default: false,
    },
    {
      command: '--sln [string]',
      description: 'Solution file to build, e.g. windows\\myApp.sln',
      default: undefined,
    },
    {
      command: '--msbuildprops [string]',
      description:
        'Comma separated props to pass to msbuild, eg: prop1=value1,prop2=value2',
    },
    {
      command: '--info',
      description: 'Dump environment information',
      default: false,
    },
    {
      command: '--autolink',
      description: 'Auto link native modules',
      default: false,
    },
    {
      command: '--direct-debugging [number]',
      description: 'Enable direct debugging on specified port',
    },
  ],
};
