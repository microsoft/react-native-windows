/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {spawn, execSync, SpawnOptions} from 'child_process';
import * as fs from 'fs';
import * as http from 'http';
import * as path from 'path';
import * as glob from 'glob';
import * as parse from 'xml-parser';
import WinAppDeployTool from './winappdeploytool';
import {
  newInfo,
  newSuccess,
  newError,
  newWarn,
  newSpinner,
  commandWithProgress,
  runPowerShellScriptFunction,
} from './commandWithProgress';
import * as build from './build';
import {BuildConfig, RunWindowsOptions} from '../runWindowsOptions';
import MSBuildTools from './msbuildtools';
import {Config} from '@react-native-community/cli-types';
import {WindowsProjectConfig} from '../../config/projectConfig';

function pushd(pathArg: string): () => void {
  const cwd = process.cwd();
  process.chdir(pathArg);
  return () => process.chdir(cwd);
}

export function getBuildConfiguration(options: RunWindowsOptions): BuildConfig {
  return options.release
    ? options.bundle
      ? 'ReleaseBundle'
      : 'Release'
    : options.bundle
    ? 'DebugBundle'
    : 'Debug';
}

function shouldLaunchApp(options: RunWindowsOptions): boolean {
  return options.launch;
}

function getAppPackage(
  options: RunWindowsOptions,
  projectName?: string,
): string {
  const configuration = getBuildConfiguration(options);
  const packageFolder =
    options.arch === 'x86'
      ? `{*_x86_${configuration}_*,*_Win32_${configuration}_*}`
      : `*_${options.arch}_${configuration}_*`;

  const appPackageGlob = `${options.root}/windows/{*/AppPackages,AppPackages/*}/${packageFolder}`;
  const appPackageCandidates = glob.sync(appPackageGlob);
  let appPackage;
  if (appPackageCandidates.length === 1 || !projectName) {
    appPackage = appPackageCandidates[0];
  } else if (appPackageCandidates.length > 1) {
    const filteredAppPackageCandidates = appPackageCandidates.filter(x =>
      x.includes(projectName),
    );
    if (filteredAppPackageCandidates.length >= 1) {
      appPackage = filteredAppPackageCandidates[0];
    }
  }

  if (!appPackage && options.release) {
    // in the latest vs, Release is removed
    newWarn(
      'No package found in *_Release_* folder, removing the _Release_ prefix and checking again',
    );

    const rootGlob = `${options.root}/windows/{*/AppPackages,AppPackages/*}`;
    const newGlob = `${rootGlob}/*_${
      options.arch === 'x86' ? 'Win32' : options.arch
    }_${options.release ? '' : 'Debug_'}Test`;

    const result = glob.sync(newGlob);
    if (result.length > 1 && projectName) {
      const newFilteredGlobs = result.filter(x => x.includes(projectName));
      if (newFilteredGlobs.length >= 1) {
        newWarn(`More than one app package found: ${result}`);
      }
      appPackage = newFilteredGlobs[0];
    } else if (result.length === 1) {
      // we're good
      appPackage = result[0];
    }
  }

  if (!appPackage) {
    throw new Error(
      `Unable to find app package using search path: "${appPackageGlob}"`,
    );
  }
  return appPackage;
}

function getWindowsStoreAppUtils(options: RunWindowsOptions) {
  const popd = pushd(options.root);
  const windowsStoreAppUtilsPath = path.resolve(
    __dirname,
    '..',
    '..',
    '..',
    'powershell',
    'WindowsStoreAppUtils.ps1',
  );
  execSync(`powershell -NoProfile Unblock-File "${windowsStoreAppUtilsPath}"`);
  popd();
  return windowsStoreAppUtilsPath;
}

function getAppxManifestPath(
  options: RunWindowsOptions,
  projectName?: string,
): string {
  const configuration = getBuildConfiguration(options);
  const appxManifestGlob = `windows/{*/bin/${options.arch}/${configuration},${configuration}/*,target/${options.arch}/${configuration},${options.arch}/${configuration}/*}/AppxManifest.xml`;
  const globs = glob.sync(path.join(options.root, appxManifestGlob));
  let appxPath: string;
  if (globs.length === 1 || !projectName) {
    appxPath = globs[0];
  } else {
    const filteredGlobs = globs.filter(x => x.indexOf(projectName) !== -1);
    if (filteredGlobs.length > 1) {
      newWarn(
        `More than one appxmanifest for ${projectName}: ${filteredGlobs.join(
          ',',
        )}`,
      );
    }
    appxPath = filteredGlobs[0];
  }

  if (!appxPath) {
    throw new Error(
      `Unable to find AppxManifest from "${options.root}", using search path: "${appxManifestGlob}" `,
    );
  }
  return appxPath;
}

function parseAppxManifest(appxManifestPath: string): parse.Document {
  return parse(fs.readFileSync(appxManifestPath, 'utf8'));
}

function getAppxManifest(options: RunWindowsOptions): parse.Document {
  return parseAppxManifest(getAppxManifestPath(options, undefined));
}

function handleResponseError(e: Error): never {
  if (e.message.indexOf('Error code -2146233088')) {
    throw new Error(`No Windows Mobile device was detected: ${e.message}`);
  } else {
    throw new Error(`Unexpected error deploying app: ${e.message}`);
  }
}

// Errors: 0x80073d10 - bad architecture
export async function deployToDevice(
  options: RunWindowsOptions,
  verbose: boolean,
) {
  const appPackageFolder = getAppPackage(options);

  const deployTarget = options.target
    ? options.target
    : options.emulator
    ? 'emulator'
    : 'device';
  const deployTool = new WinAppDeployTool();
  const appxManifest = getAppxManifest(options);
  const shouldLaunch = shouldLaunchApp(options);
  const identity = appxManifest.root.children.filter(function(x) {
    return x.name === 'mp:PhoneIdentity';
  })[0];
  const appName = identity.attributes.PhoneProductId;

  const device = deployTool.findDevice(deployTarget);

  try {
    await deployTool.uninstallAppPackage(appName, device, verbose);
  } catch (e) {
    newWarn('Failed to uninstall app from ' + device.name);
  }

  const appxFile = glob.sync(path.join(appPackageFolder, '*.appx'))[0];
  try {
    await deployTool.installAppPackage(
      appxFile,
      device,
      shouldLaunch,
      false,
      verbose,
    );
  } catch (e) {
    if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
      await deployTool.installAppPackage(
        appxFile,
        device,
        shouldLaunch,
        true,
        verbose,
      );
    } else {
      handleResponseError(e);
    }
  }
}

export async function deployToDesktop(
  options: RunWindowsOptions,
  verbose: boolean,
  config: Config,
  buildTools: MSBuildTools,
) {
  const windowsConfig: Partial<WindowsProjectConfig> | undefined =
    config.project.windows;
  const slnFile =
    windowsConfig && windowsConfig.solutionFile && windowsConfig.sourceDir
      ? path.join(windowsConfig.sourceDir, windowsConfig.solutionFile)
      : options.sln!;
  const projectName =
    windowsConfig && windowsConfig.project && windowsConfig.project.projectName
      ? windowsConfig.project.projectName
      : options.proj!;
  const appPackageFolder = getAppPackage(options, projectName);
  const windowsStoreAppUtils = getWindowsStoreAppUtils(options);
  const appxManifestPath = getAppxManifestPath(options, projectName);
  const appxManifest = parseAppxManifest(appxManifestPath);
  const identity = appxManifest.root.children.filter(function(x) {
    return x.name === 'Identity';
  })[0];
  const appName = identity.attributes.Name;
  const script = glob.sync(
    path.join(appPackageFolder, 'Add-AppDevPackage.ps1'),
  )[0];

  const vsVersion = buildTools.installationVersion;

  if (vsVersion.startsWith('16.5') || vsVersion.startsWith('16.6')) {
    // VS 16.5 and 16.6 introduced a regression in packaging where the certificates created in the UI will render the package uninstallable.
    // This will be fixed in 16.7. In the meantime we need to copy the Add-AppDevPackage that has the fix for this EKU issue:
    // https://developercommunity.visualstudio.com/content/problem/1012921/uwp-packaging-generates-incompatible-certificate.html
    if (verbose) {
      newWarn(
        'Applying Add-AppDevPackage.ps1 workaround for VS 16.5-16.6 bug - see https://developercommunity.visualstudio.com/content/problem/1012921/uwp-packaging-generates-incompatible-certificate.html',
      );
    }
    fs.copyFileSync(
      path.join(
        path.resolve(__dirname),
        '..',
        '..',
        '..',
        'powershell',
        'Add-AppDevPackage.ps1',
      ),
      script,
    );
  }

  let args = [];
  if (options.remoteDebugging) {
    args.push('--remote-debugging');
  }

  if (options.directDebugging) {
    args.push('--direct-debugging', options.directDebugging.toString());
  }

  await runPowerShellScriptFunction(
    'Removing old version of the app',
    windowsStoreAppUtils,
    `Uninstall-App ${appName}`,
    verbose,
  );

  await runPowerShellScriptFunction(
    'Enabling Developer Mode',
    windowsStoreAppUtils,
    'EnableDevMode',
    verbose,
  );

  if (options.release) {
    await runPowerShellScriptFunction(
      'Installing new version of the app',
      windowsStoreAppUtils,
      `Install-App "${script}" -Force`,
      verbose,
    );
  } else {
    // Install the app package's dependencies before attempting to deploy.
    await runPowerShellScriptFunction(
      'Installing dependent framework packages',
      windowsStoreAppUtils,
      `Install-AppDependencies ${appxManifestPath} ${appPackageFolder} ${options.arch}`,
      verbose,
    );
    await build.buildSolution(
      buildTools,
      slnFile,
      options.release ? 'Release' : 'Debug',
      options.arch,
      {DeployLayout: 'true'},
      verbose,
      'Deploy',
      options.buildLogDirectory,
    );
  }

  const appFamilyName = execSync(
    `powershell -NoProfile -c $(Get-AppxPackage -Name ${appName}).PackageFamilyName`,
  )
    .toString()
    .trim();

  if (!appFamilyName) {
    throw new Error(
      'Fail to check the installed app, maybe developer mode is off on Windows',
    );
  }

  const loopbackText = 'Verifying loopbackExempt';
  const loopbackSpinner = newSpinner(loopbackText);

  await commandWithProgress(
    loopbackSpinner,
    loopbackText,
    'CheckNetIsolation',
    `LoopbackExempt -a -n=${appFamilyName}`.split(' '),
    verbose,
  );

  if (shouldLaunchApp(options)) {
    await runPowerShellScriptFunction(
      'Starting the app',
      windowsStoreAppUtils,
      `Start-Locally ${appName} ${args}`,
      verbose,
    );
  } else {
    newInfo('Skip the step to start the app');
  }
}

export function startServerInNewWindow(
  options: RunWindowsOptions,
  verbose: boolean,
): Promise<void> {
  return new Promise(resolve => {
    if (options.packager) {
      http
        .get('http://localhost:8081/status', res => {
          if (res.statusCode === 200) {
            newSuccess('React-Native Server already started');
          } else {
            newError('React-Native Server not responding');
          }
          resolve();
        })
        .on('error', () => {
          launchServer(options, verbose);
          resolve();
        });
    } else {
      resolve();
    }
  });
}

function launchServer(options: RunWindowsOptions, verbose: boolean) {
  newSuccess('Starting the React-Native Server');
  const opts: SpawnOptions = {
    cwd: options.root,
    detached: true,
    stdio: verbose ? 'inherit' : 'ignore',
  };

  spawn('cmd.exe', ['/C', 'start npx --no-install react-native start'], opts);
}
