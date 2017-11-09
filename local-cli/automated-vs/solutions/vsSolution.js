 /*
  * Some descendant of this class needs to have:
  * deploy
  */

'use-strict';

const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const fs = require('fs');
const shell = require('shelljs');
const child_process = require('child_process');
const EOL = require('os').EOL;
const nuget = require('../../nuget-exe/index');
const MSBuild = require('../utils/msbuild');
const MSBuildTools = MSBuild.MSBuildTools;
const VS2017 = require('../visualStudios/vs2017');
const VS2015OrEarlier = require('../visualStudios/vs2015OrEarlier');
const utils = require('../utils/utility');

module.exports = class VSSolution {
    constructor(slnFile, root) {
        this.slnFile = slnFile;
        this.root = root;
        this.name = this.getName(slnFile);
        this.visualStudio = this.createVSVersion();
        this.defaultMSBuildArgs = [
            '/clp:NoSummary;NoItemAndPropertyList',
            '/nologo',
            '/p:AppxBundle=Never'
        ];
    }

    getName(slnFile) { //get project name from sln file name
        if (!/.sln$/.test(slnFile) || !shell.test('-e', slnFile)) {
            throw new Error('Solution file (.sln) not found.');
        }

        return slnFile.slice(slnFile.search(/([^\\/:*?"<>]([a-zA-Z0-9_.]|(\\ )?))+.sln$/), -4);
    }

    //CONSTRUCTING STEPS
    createVSVersion() {
        //if vswhere.exe exists, using Visual Studio 2017
        const vsWherePath = path.join(process.env['ProgramFiles(x86)'], '/Microsoft Visual Studio/Installer/vswhere.exe');
        if (fs.existsSync(vsWherePath)) {
            return new VS2017();
        } else {
            return new VS2015OrEarlier();
        }

    }

    getVSVersion() {
        return this.visualStudio.getVSVersion();
    }

    //BUILD STEPS
    build(options, args) {
        options.configuration = utils.getConfiguration(options.release);
        try {
            this.restoreNuGetPackages();
        } catch (e) {
            throw new Error('Failed to restore the NuGet packages. Error:' + e.message);
        }
        this.checkAndGetRequirements(options.verbose);
        this.buildProject(this.slnFile, options, args);
    }

    restoreNuGetPackages(verbose) {
        let nugetPath = path.join(this.root, nuget.getNuGetPath());

        console.log(chalk.green('Restoring NuGet packages'));
        const verboseOption = utils.getVerboseCLP(verbose);
        // Always inherit from stdio as we're controlling verbosity output above.
        execSync(`"${nugetPath}" restore "${this.slnFile}" -NonInteractive -Verbosity ${verboseOption}`, { stdio: 'inherit' });
    }

    checkAndGetRequirements() {
        this.visualStudio.msBuild = this.hasMSBuild();
    }

    hasMSBuild() {
        const versions = MSBuild.MSBUILD_VERSIONS.map(this.checkMSBuildVersion, this);
        const msbuildTools = versions[0] || versions[1] || versions[2] || versions[3];
        if (!msbuildTools) {
          throw new Error('MSBuild tools not found');
        }

        return msbuildTools;
    }

    checkMSBuildVersion(version) {
        let toolsPath = this.visualStudio.getMSBuildToolsPath(version);

        // We found something so return MSBuild Tools.
        if (toolsPath){
            console.log(chalk.green(`Found MSBuild v${version} at ${toolsPath}`));
            return new MSBuildTools(version, toolsPath);
        }
        else {
            return null;
        }
    }

    buildProject(slnFile, options, args) {
        console.log(chalk.green(`Building Solution: ${slnFile}`));
        console.log(chalk.green(`Build configuration: ${options.configuration}`));
        console.log(chalk.green(`Build platform: ${options.arch}`));

        //if using default args, add user's choice of configuration, arch, and verbosity
        if (!args) {
            args = this.defaultMSBuildArgs;
            args.push(`/p:Configuration=${options.configuration}`);
            args.push(`/p:Platform=${options.arch}`);
            args.push(`/clp:Verbosity=${utils.getVerboseCLP(options.verbose)}`);
        }

        const msbuildpath = this.visualStudio.msBuild.path;

        const cmd = `"${path.join(msbuildpath, 'msbuild.exe')}" ` + ['"' + slnFile + '"'].concat(args).join(' ');
        child_process.execSync(cmd, { stdio: 'inherit' });
    }

    cleanProject() {
        const cmd = `"${path.join(this.visualStudio.msBuild.path, 'msbuild.exe')}" "${this.slnFile}" t/:Clean`;
        const results = child_process.execSync(cmd).toString().split(EOL);
        results.forEach(result => console.log(chalk.white(result)));
    }

    //DEPLOYING STEPS
    deploy(options) {
        options.configuration = utils.getConfiguration(options.release);
    }

    pushd(pathArg) {
        const cwd = process.cwd();
        process.chdir(pathArg);
        return () => process.chdir(cwd);
    }
};
