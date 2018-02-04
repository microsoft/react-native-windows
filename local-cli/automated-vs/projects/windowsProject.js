/*
 * Some descendant of this class must have:
 * getWindowsSDKFolderPath
 */

'use-strict';

const execSync = require('child_process').execSync;
const shell = require('shelljs');
const path = require('path');
const Version = require('../utils/version');
const VSProject = require('./vsProject');

module.exports = class WindowsProject extends VSProject {
    constructor(slnFile, root, visualStudio, minWinSDK) {
        super(slnFile, root, visualStudio);
        this.minWinSDK = minWinSDK;
    }

    //build
    checkAndGetRequirements() {
        super.checkAndGetRequirements();
        this.checkWinSDK(this.minWinSDK);
    }

    checkWinSDK(version) {
        const minVersion = Version.fromString(version);
        const allVersions = this.getAllAvailableWinSDKVersions();

        if (!allVersions.some(v => v.gte(minVersion))) {
            throw new Error('Must have a minimum Windows SDK version' + minVersion + 'installed');
          }
    }

    getAllAvailableWinSDKVersions() {
        let results = this.checkForWinSDKsInProgramFiles();
        if (results.length <= 0) {
            results = this.checkForWinSDKsInReg();
        }

        return results;
    }

    checkForWinSDKsInProgramFiles() {
        const installedSdks = [];

        const programFilesFolder = process.env['ProgramFiles(x86)'] || process.env.ProgramFiles;

        if (!programFilesFolder) {
            return installedSdks;
        }

        var folderPath = this.getWindowsSDKFolderPath(programFilesFolder);

        if (!shell.test('-e', folderPath)) { //if folder path doesn't exist
            return installedSdks;
        }

        shell.ls(folderPath)
            .filter(uapDir => shell.test('-d', path.join(folderPath, uapDir))) //if path is a directory
            .map(Version.tryParse)
            .forEach(version => version && installedSdks.push(version));

        return installedSdks;
    }

    checkForWinSDKsInReg() { //keeping the registry check for generalizability but the user will have to specify a less specific version of win sdk - only gets 10 or 8.1
        const installedSdks = [];
        const execString = 'reg query "HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows" /s /v InstallationFolder /reg:32';
        let output;
        try {
            output = execSync(execString).toString();
        } catch (e) {
            return installedSdks;
        }

        const re = /\\Microsoft SDKs\\Windows\\v(\d+\.\d+)\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
        let match;
        while ((match = re.exec(output))) {
            const sdkPath = match[2];
            if (shell.test('-e', path.join(sdkPath, 'SDKManifest.xml'))) {
                installedSdks.push(Version.tryParse(match[1]));
            }
        }

        return installedSdks;
    }
};
