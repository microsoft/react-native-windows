"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const os_1 = require("os");
const fs = require("fs");
const path = require("path");
const child_process = require("child_process");
const chalk = require("chalk");
const os = require("os");
const shell = require("shelljs");
const version_1 = require("./version");
const checkRequirements = require("./checkRequirements");
const commandWithProgress_1 = require("./commandWithProgress");
const child_process_1 = require("child_process");
const vsInstalls_1 = require("./vsInstalls");
class MSBuildTools {
    /**
     * @param version is something like 16.0 for 2019
     * @param msbuildPath  Path to MSBuild.exe (x86)
     * @param installationVersion is the full version e.g. 16.3.29411.108
     */
    constructor(version, msbuildPath, installationVersion) {
        this.version = version;
        this.msbuildPath = msbuildPath;
        this.installationVersion = installationVersion;
    }
    cleanProject(slnFile) {
        const cmd = `"${path.join(this.msbuildPath, 'msbuild.exe')}" "${slnFile}" /t:Clean`;
        const results = child_process
            .execSync(cmd)
            .toString()
            .split(os_1.EOL);
        results.forEach(result => console.log(chalk.white(result)));
    }
    async buildProject(slnFile, buildType, buildArch, msBuildProps, verbose, target, buildLogDirectory) {
        commandWithProgress_1.newSuccess(`Found Solution: ${slnFile}`);
        commandWithProgress_1.newInfo(`Build configuration: ${buildType}`);
        commandWithProgress_1.newInfo(`Build platform: ${buildArch}`);
        if (target) {
            commandWithProgress_1.newInfo(`Build target: ${target}`);
        }
        const verbosityOption = verbose ? 'normal' : 'minimal';
        const logPrefix = path.join(buildLogDirectory || os.tmpdir(), `msbuild_${process.pid}${target ? '_' + target : ''}`);
        const errorLog = logPrefix + '.err';
        const warnLog = logPrefix + '.wrn';
        const localBinLog = target ? `:${target}.binlog` : '';
        const binlog = buildLogDirectory ? `:${logPrefix}.binlog` : localBinLog;
        const args = [
            `/clp:NoSummary;NoItemAndPropertyList;Verbosity=${verbosityOption}`,
            '/nologo',
            '/maxCpuCount',
            `/p:Configuration=${buildType}`,
            `/p:Platform=${buildArch}`,
            '/p:AppxBundle=Never',
            `/bl${binlog}`,
            `/flp1:errorsonly;logfile=${errorLog}`,
            `/flp2:warningsonly;logfile=${warnLog}`,
        ];
        if (target) {
            args.push(`/t:${target}`);
        }
        if (msBuildProps) {
            Object.keys(msBuildProps).forEach(function (key) {
                args.push(`/p:${key}=${msBuildProps[key]}`);
            });
        }
        try {
            checkRequirements.isWinSdkPresent('10.0');
        }
        catch (e) {
            commandWithProgress_1.newError(e.message);
            throw e;
        }
        if (verbose) {
            console.log(`Running MSBuild with args ${args.join(' ')}`);
        }
        const progressName = 'Building Solution';
        const spinner = commandWithProgress_1.newSpinner(progressName);
        try {
            await commandWithProgress_1.commandWithProgress(spinner, progressName, path.join(this.msbuildPath, 'msbuild.exe'), [slnFile].concat(args), verbose);
        }
        catch (e) {
            let error = e;
            if (!e) {
                const firstMessage = (await fs.promises.readFile(errorLog))
                    .toString()
                    .split(os_1.EOL)[0];
                error = new Error(firstMessage);
                error.logfile = errorLog;
            }
            throw error;
        }
        // If we have no errors, delete the error log when we're done
        if ((await fs.promises.stat(errorLog)).size === 0) {
            await fs.promises.unlink(errorLog);
        }
    }
    static findAvailableVersion(buildArch, verbose) {
        // https://aka.ms/vs/workloads
        const requires = [
            'Microsoft.Component.MSBuild',
            getVCToolsByArch(buildArch),
        ];
        const version = process.env.VisualStudioVersion || '16.0';
        const vsInstallation = vsInstalls_1.findLatestVsInstall({ requires, version, verbose });
        if (!vsInstallation) {
            if (process.env.VisualStudioVersion != null) {
                throw new Error(`MSBuild tools not found for version ${process.env.VisualStudioVersion} (from environment). Make sure all required components have been installed`);
            }
            else {
                throw new Error('MSBuild tools not found. Make sure all required components have been installed');
            }
        }
        const toolsPath = path.join(vsInstallation.installationPath, 'MSBuild/Current/Bin');
        if (fs.existsSync(toolsPath)) {
            commandWithProgress_1.newSuccess(`Found MSBuild v${version} at ${toolsPath} (${vsInstallation.installationVersion})`);
            return new MSBuildTools(version, toolsPath, vsInstallation.installationVersion);
        }
        else {
            throw new Error(`MSBuild path '${toolsPath} does not exist'`);
        }
    }
    static getAllAvailableUAPVersions() {
        const results = [];
        const programFilesFolder = process.env['ProgramFiles(x86)'] || process.env.ProgramFiles;
        // No Program Files folder found, so we won't be able to find UAP SDK
        if (!programFilesFolder) {
            return results;
        }
        let uapFolderPath = path.join(programFilesFolder, 'Windows Kits', '10', 'Platforms', 'UAP');
        if (!shell.test('-e', uapFolderPath)) {
            // Check other installation folder from reg
            const sdkFolder = getSDK10InstallationFolder();
            if (sdkFolder) {
                uapFolderPath = path.join(sdkFolder, 'Platforms', 'UAP');
            }
        }
        // No UAP SDK exists on this machine
        if (!shell.test('-e', uapFolderPath)) {
            return results;
        }
        shell
            .ls(uapFolderPath)
            .filter(uapDir => shell.test('-d', path.join(uapFolderPath, uapDir)))
            .map(version_1.default.tryParse)
            .forEach(version => version && results.push(version));
        return results;
    }
}
exports.default = MSBuildTools;
function getVCToolsByArch(buildArch) {
    switch (buildArch) {
        case 'x86':
        case 'x64':
            return 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64';
        case 'ARM':
            return 'Microsoft.VisualStudio.Component.VC.Tools.ARM';
        case 'ARM64':
            return 'Microsoft.VisualStudio.Component.VC.Tools.ARM64';
    }
}
function getSDK10InstallationFolder() {
    const folder = '';
    const execString = 'reg query "HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v10.0" /s /v InstallationFolder /reg:32';
    let output;
    try {
        output = child_process_1.execSync(execString).toString();
    }
    catch (e) {
        return folder;
    }
    const re = /\\Microsoft SDKs\\Windows\\v10.0\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
    const match = re.exec(output);
    if (match) {
        return match[1];
    }
    return folder;
}
//# sourceMappingURL=msbuildtools.js.map