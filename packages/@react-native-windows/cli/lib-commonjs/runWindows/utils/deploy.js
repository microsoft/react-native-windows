"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.startServerInNewWindow = exports.deployToDesktop = exports.deployToDevice = exports.getBuildConfiguration = void 0;
const child_process_1 = require("child_process");
const fs = require("fs");
const http = require("http");
const path = require("path");
const glob = require("glob");
const parse = require("xml-parser");
const winappdeploytool_1 = require("./winappdeploytool");
const commandWithProgress_1 = require("./commandWithProgress");
const build = require("./build");
function pushd(pathArg) {
    const cwd = process.cwd();
    process.chdir(pathArg);
    return () => process.chdir(cwd);
}
function getBuildConfiguration(options) {
    return options.release
        ? options.bundle
            ? 'ReleaseBundle'
            : 'Release'
        : options.bundle
            ? 'DebugBundle'
            : 'Debug';
}
exports.getBuildConfiguration = getBuildConfiguration;
function shouldLaunchApp(options) {
    return options.launch;
}
function getAppPackage(options, projectName) {
    const configuration = getBuildConfiguration(options);
    const packageFolder = options.arch === 'x86'
        ? `{*_x86_${configuration}_*,*_Win32_${configuration}_*}`
        : `*_${options.arch}_${configuration}_*`;
    const appPackageGlob = `${options.root}/windows/{*/AppPackages,AppPackages/*}/${packageFolder}`;
    const appPackageCandidates = glob.sync(appPackageGlob);
    let appPackage;
    if (appPackageCandidates.length === 1 || !projectName) {
        appPackage = appPackageCandidates[0];
    }
    else if (appPackageCandidates.length > 1) {
        const filteredAppPackageCandidates = appPackageCandidates.filter(x => x.includes(projectName));
        if (filteredAppPackageCandidates.length >= 1) {
            appPackage = filteredAppPackageCandidates[0];
        }
    }
    if (!appPackage && options.release) {
        // in the latest vs, Release is removed
        commandWithProgress_1.newWarn('No package found in *_Release_* folder, removing the _Release_ prefix and checking again');
        const rootGlob = `${options.root}/windows/{*/AppPackages,AppPackages/*}`;
        const newGlob = `${rootGlob}/*_${options.arch === 'x86' ? 'Win32' : options.arch}_${options.release ? '' : 'Debug_'}Test`;
        const result = glob.sync(newGlob);
        if (result.length > 1 && projectName) {
            const newFilteredGlobs = result.filter(x => x.includes(projectName));
            if (newFilteredGlobs.length >= 1) {
                commandWithProgress_1.newWarn(`More than one app package found: ${result}`);
            }
            appPackage = newFilteredGlobs[0];
        }
        else if (result.length === 1) {
            // we're good
            appPackage = result[0];
        }
    }
    if (!appPackage) {
        throw new Error(`Unable to find app package using search path: "${appPackageGlob}"`);
    }
    return appPackage;
}
function getWindowsStoreAppUtils(options) {
    const popd = pushd(options.root);
    const windowsStoreAppUtilsPath = path.resolve(__dirname, '..', '..', '..', 'powershell', 'WindowsStoreAppUtils.ps1');
    child_process_1.execSync(`powershell Unblock-File "${windowsStoreAppUtilsPath}"`);
    popd();
    return windowsStoreAppUtilsPath;
}
function getAppxManifestPath(options, projectName) {
    const configuration = getBuildConfiguration(options);
    const appxManifestGlob = `windows/{*/bin/${options.arch}/${configuration},${configuration}/*,target/${options.arch}/${configuration},${options.arch}/${configuration}/*}/AppxManifest.xml`;
    const globs = glob.sync(path.join(options.root, appxManifestGlob));
    let appxPath;
    if (globs.length === 1 || !projectName) {
        appxPath = globs[0];
    }
    else {
        const filteredGlobs = globs.filter(x => x.indexOf(projectName) !== -1);
        if (filteredGlobs.length > 1) {
            commandWithProgress_1.newWarn(`More than one appxmanifest for ${projectName}: ${filteredGlobs.join(',')}`);
        }
        appxPath = filteredGlobs[0];
    }
    if (!appxPath) {
        throw new Error(`Unable to find AppxManifest from "${options.root}", using search path: "${appxManifestGlob}" `);
    }
    return appxPath;
}
function parseAppxManifest(appxManifestPath) {
    return parse(fs.readFileSync(appxManifestPath, 'utf8'));
}
function getAppxManifest(options) {
    return parseAppxManifest(getAppxManifestPath(options, undefined));
}
function handleResponseError(e) {
    if (e.message.indexOf('Error code -2146233088')) {
        throw new Error(`No Windows Mobile device was detected: ${e.message}`);
    }
    else {
        throw new Error(`Unexpected error deploying app: ${e.message}`);
    }
}
// Errors: 0x80073d10 - bad architecture
async function deployToDevice(options, verbose) {
    const appPackageFolder = getAppPackage(options);
    const deployTarget = options.target
        ? options.target
        : options.emulator
            ? 'emulator'
            : 'device';
    const deployTool = new winappdeploytool_1.default();
    const appxManifest = getAppxManifest(options);
    const shouldLaunch = shouldLaunchApp(options);
    const identity = appxManifest.root.children.filter(function (x) {
        return x.name === 'mp:PhoneIdentity';
    })[0];
    const appName = identity.attributes.PhoneProductId;
    const device = deployTool.findDevice(deployTarget);
    try {
        await deployTool.uninstallAppPackage(appName, device, verbose);
    }
    catch (e) {
        commandWithProgress_1.newWarn('Failed to uninstall app from ' + device.name);
    }
    const appxFile = glob.sync(path.join(appPackageFolder, '*.appx'))[0];
    try {
        await deployTool.installAppPackage(appxFile, device, shouldLaunch, false, verbose);
    }
    catch (e) {
        if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
            await deployTool.installAppPackage(appxFile, device, shouldLaunch, true, verbose);
        }
        else {
            handleResponseError(e);
        }
    }
}
exports.deployToDevice = deployToDevice;
async function deployToDesktop(options, verbose, config, buildTools) {
    const windowsConfig = config.project.windows;
    const slnFile = windowsConfig && windowsConfig.solutionFile && windowsConfig.sourceDir
        ? path.join(windowsConfig.sourceDir, windowsConfig.solutionFile)
        : options.sln;
    const projectName = windowsConfig && windowsConfig.project && windowsConfig.project.projectName
        ? windowsConfig.project.projectName
        : options.proj;
    const appPackageFolder = getAppPackage(options, projectName);
    const windowsStoreAppUtils = getWindowsStoreAppUtils(options);
    const appxManifestPath = getAppxManifestPath(options, projectName);
    const appxManifest = parseAppxManifest(appxManifestPath);
    const identity = appxManifest.root.children.filter(function (x) {
        return x.name === 'Identity';
    })[0];
    const appName = identity.attributes.Name;
    const script = glob.sync(path.join(appPackageFolder, 'Add-AppDevPackage.ps1'))[0];
    const vsVersion = buildTools.installationVersion;
    if (vsVersion.startsWith('16.5') || vsVersion.startsWith('16.6')) {
        // VS 16.5 and 16.6 introduced a regression in packaging where the certificates created in the UI will render the package uninstallable.
        // This will be fixed in 16.7. In the meantime we need to copy the Add-AppDevPackage that has the fix for this EKU issue:
        // https://developercommunity.visualstudio.com/content/problem/1012921/uwp-packaging-generates-incompatible-certificate.html
        if (verbose) {
            commandWithProgress_1.newWarn('Applying Add-AppDevPackage.ps1 workaround for VS 16.5-16.6 bug - see https://developercommunity.visualstudio.com/content/problem/1012921/uwp-packaging-generates-incompatible-certificate.html');
        }
        fs.copyFileSync(path.join(path.resolve(__dirname), '..', '..', '..', 'powershell', 'Add-AppDevPackage.ps1'), script);
    }
    let args = [];
    if (options.remoteDebugging) {
        args.push('--remote-debugging');
    }
    if (options.directDebugging) {
        args.push('--direct-debugging', options.directDebugging.toString());
    }
    await commandWithProgress_1.runPowerShellScriptFunction('Removing old version of the app', windowsStoreAppUtils, `Uninstall-App ${appName}`, verbose);
    await commandWithProgress_1.runPowerShellScriptFunction('Enabling Developer Mode', windowsStoreAppUtils, 'EnableDevMode', verbose);
    if (options.release) {
        await commandWithProgress_1.runPowerShellScriptFunction('Installing new version of the app', windowsStoreAppUtils, `Install-App "${script}" -Force`, verbose);
    }
    else {
        // Install the app package's dependencies before attempting to deploy.
        await commandWithProgress_1.runPowerShellScriptFunction('Installing dependent framework packages', windowsStoreAppUtils, `Install-AppDependencies ${appxManifestPath} ${appPackageFolder} ${options.arch}`, verbose);
        await build.buildSolution(buildTools, slnFile, options.release ? 'Release' : 'Debug', options.arch, { DeployLayout: 'true' }, verbose, 'Deploy', options.buildLogDirectory);
    }
    const appFamilyName = child_process_1.execSync(`powershell -c $(Get-AppxPackage -Name ${appName}).PackageFamilyName`)
        .toString()
        .trim();
    if (!appFamilyName) {
        throw new Error('Fail to check the installed app, maybe developer mode is off on Windows');
    }
    const loopbackText = 'Verifying loopbackExempt';
    const loopbackSpinner = commandWithProgress_1.newSpinner(loopbackText);
    await commandWithProgress_1.commandWithProgress(loopbackSpinner, loopbackText, 'CheckNetIsolation', `LoopbackExempt -a -n=${appFamilyName}`.split(' '), verbose);
    if (shouldLaunchApp(options)) {
        await commandWithProgress_1.runPowerShellScriptFunction('Starting the app', windowsStoreAppUtils, `Start-Locally ${appName} ${args}`, verbose);
    }
    else {
        commandWithProgress_1.newInfo('Skip the step to start the app');
    }
}
exports.deployToDesktop = deployToDesktop;
function startServerInNewWindow(options, verbose) {
    return new Promise(resolve => {
        if (options.packager) {
            http
                .get('http://localhost:8081/status', res => {
                if (res.statusCode === 200) {
                    commandWithProgress_1.newSuccess('React-Native Server already started');
                }
                else {
                    commandWithProgress_1.newError('React-Native Server not responding');
                }
                resolve();
            })
                .on('error', () => {
                launchServer(options, verbose);
                resolve();
            });
        }
        else {
            resolve();
        }
    });
}
exports.startServerInNewWindow = startServerInNewWindow;
function launchServer(options, verbose) {
    commandWithProgress_1.newSuccess('Starting the React-Native Server');
    const opts = {
        cwd: options.root,
        detached: true,
        stdio: verbose ? 'inherit' : 'ignore',
    };
    child_process_1.spawn('cmd.exe', ['/C', 'start npx --no-install react-native start'], opts);
}
//# sourceMappingURL=deploy.js.map