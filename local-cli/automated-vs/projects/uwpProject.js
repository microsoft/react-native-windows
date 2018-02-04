'use-strict';

const chalk = require('chalk');
const path = require('path');
const glob = require('glob');
const child_process = require('child_process');
const execSync = child_process.execSync;
const parse = require('xml-parser');
const fs = require('fs');
const WindowsProject = require('./windowsProject');
const WinAppDeployTool = require('../utils/winappdeploytool');

module.exports = class UWPProject extends WindowsProject {

    constructor(slnFile, root, visualStudio, minWinSDK) {
        super(slnFile, root, visualStudio, minWinSDK);

        this.defaultMSBuildArgs.push('/p:AppxBundle=Never');
    }

    //build
    getWindowsSDKFolderPath(programFilesFolder) {
        return path.join(programFilesFolder, 'Windows Kits', '10', 'Platforms', 'UAP');
    }

    //deploy
    deploy(options, alternateWindowsPath) {
        super.deploy(options);
        if (options.device || options.emulator || options.target) {
            return this.deployToDevice(options, alternateWindowsPath);
        } else {
            return this.deployToDesktop(options, alternateWindowsPath);
        }
    }

    deployToDesktop(options, alternateWindowsPath) {
        const appPackageFolder = this.getAppPackageFolder(options.arch, options.configuration, alternateWindowsPath);
        const script = glob.sync(path.join(appPackageFolder, 'Add-AppDevPackage.ps1'))[0];
        const windowsStoreAppUtils = this.getWindowsStoreAppUtils();
        const appxManifest = this.getAppxManifest(options.arch, options.configuration, alternateWindowsPath);
        const identity = appxManifest.root.children.filter(function (x) { return x.name === 'Identity'; })[0];
        const appName = identity.attributes.Name;

        const args = ['remoteDebugging', options.proxy ? 'true' : 'false'];
        const execOptions = options.verbose ? { stdio: 'inherit' } : {};

        return new Promise(resolve => {
            const popd = this.pushd(options.root);

            console.log(chalk.green('Removing old version of the app'));
            execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}" ; Uninstall-App ${appName}`, execOptions);

            console.log(chalk.green('Installing new version of the app'));
            execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Install-App "${script}"`, execOptions);

            const appFamilyName = execSync(`powershell -c $(Get-AppxPackage -Name ${appName}).PackageFamilyName`).toString().trim();
            execSync(`CheckNetIsolation LoopbackExempt -a -n=${appFamilyName}`, execOptions);

            console.log(chalk.green('Starting the app'));
            execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Start-Locally ${appName} ${args}`, execOptions);

            popd();

            resolve();
        });
    }

    deployToDevice(options, alternateWindowsPath) {
        const appPackageFolder = this.getAppPackageFolder(options.arch, options.configuration, alternateWindowsPath);
        const deployTarget = options.target ? options.target : (options.emulator ? 'emulator' : 'device');
        const deployTool = new WinAppDeployTool();
        const appxManifest = this.getAppxManifest(options.arch, options.configuration, alternateWindowsPath);
        const identity = appxManifest.root.children.filter(function (x) { return x.name === 'mp:PhoneIdentity'; })[0];
        const appName = identity.attributes.PhoneProductId;

        return new Promise(resolve => {
            const device = deployTool.findDevice(deployTarget);

            try {
                deployTool.uninstallAppPackage(appName, device, options.verbose);
            } catch (e) {
                console.log(chalk.yellow('Failed to uninstall app from ' + device.name));
            }

            const appxFile = glob.sync(path.join(appPackageFolder, '*.appx'))[0];
            try {
                console.log(chalk.white(deployTool.installAppPackage(appxFile, device, true, false, options.verbose)));
                resolve();
            } catch (e) {
                if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
                    console.log(chalk.white(deployTool.installAppPackage(appxFile, device, true, true, options.verbose)));
                    resolve();
                } else {
                    this.handleResponseError(e);
                }
            }
        });
    }

    //assumes app package is in some AppPackages/*_[arch]_[configuration]_* directory
    getAppPackageFolder(arch, configuration, alternateAppPackagePath) {
        const baseAppPackagesPath = `AppPackages/*_${arch}_*` + (configuration === 'Debug' ? `${configuration}_*` : '');
        let appPackagesPath;
        if (alternateAppPackagePath) {
            appPackagesPath = path.join(alternateAppPackagePath, baseAppPackagesPath);
        } else {
            appPackagesPath = baseAppPackagesPath;
        }

        return glob.sync(appPackagesPath)[0];
    }

    getWindowsStoreAppUtils() {
        const popd = this.pushd(this.root);
        const windowsStoreAppUtilsPath = './node_modules/react-native-windows/local-cli/automated-vs/utils/WindowsStoreAppUtils.ps1';
        execSync(`powershell Unblock-File "${windowsStoreAppUtilsPath}"`);
        popd();
        return windowsStoreAppUtilsPath;
    }

    //assumes appxmanifest is in some bin/[arch]/[configuration] directory
    getAppxManifest(arch, configuration, alternateAppxManifestPath) {
        let newRoot;
        if (alternateAppxManifestPath) {
            newRoot = path.join(this.root, alternateAppxManifestPath);
        } else {
            newRoot = this.root;
        }
        const appxPath = glob.sync(path.join(newRoot, `bin/${arch}/${configuration}/AppxManifest.xml`))[0];
        return parse(fs.readFileSync(appxPath, 'utf8'));
    }
};
