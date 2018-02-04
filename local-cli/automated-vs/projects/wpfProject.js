'use-strict';

const path = require('path');
const chalk = require('chalk');
const child_process = require('child_process');
const spawn = child_process.spawn;
const WindowsProject = require('./windowsProject');

module.exports = class WPFProject extends WindowsProject {

    //build
    getWindowsSDKFolderPath(programFilesFolder) {
        return path.join(programFilesFolder, 'Windows Kits');
    }

    //deploy
    deploy(options, alternateWindowsPath) {
        super.deploy(options);
        this.deployToDesktop(options, alternateWindowsPath);
    }

    deployToDesktop(options, alternateWindowsPath) {
        const defaultLaunchAppscript = `/bin/${options.arch}/${options.configuration}/${this.name}`;

        let launchAppScript;
        if (alternateWindowsPath) {
            launchAppScript = path.join(alternateWindowsPath, defaultLaunchAppscript);
        } else {
            launchAppScript = path.join(defaultLaunchAppscript);
        }

        console.log(chalk.green('Starting the app'));
        const spawnOptions = options.verbose ? { stdio: 'inherit' } : {};
        return Promise.resolve(spawn('cmd.exe', ['/C', 'start', launchAppScript,
          'remoteDebugging', options.proxy ? 'true' : 'false'], spawnOptions));
      }
};
