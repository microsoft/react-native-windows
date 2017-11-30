 /*
  * Some descendant of this class needs to have:
  * deploy
  */

'use-strict';

const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const shell = require('shelljs');
const child_process = require('child_process');
const EOL = require('os').EOL;
const nuget = require('../../nuget-exe/index');
const MSBuildTools = require('../utils/msbuild');
const VS2017 = require('../visualStudios/vs2017');
const VS2015OrEarlier = require('../visualStudios/vs2015OrEarlier');
const VS = require('../visualStudios/vs');
const utils = require('../utils/cliOptions');

module.exports = class VSProject {
    constructor(slnFile, root, visualStudio) {
        this.slnFile = slnFile;
        this.root = root;
        this.name = this.getName(slnFile);
        this.visualStudio = this.createVSVersion(visualStudio);
        this.defaultMSBuildArgs = [
            '/clp:NoSummary;NoItemAndPropertyList',
            '/nologo'
        ];
    }

    getName(slnFile) { //get solution name from sln file name
        if (!/.sln$/.test(slnFile) || !shell.test('-e', slnFile)) {
            throw new Error('Solution file (.sln) not found.');
        }

        return slnFile.slice(slnFile.search(/([^\\/:*?"<>]([a-zA-Z0-9_.]|(\\ )?))+.sln$/), -4);
    }

    //CONSTRUCTING STEPS
    createVSVersion(visualStudio) {
        if (visualStudio === null) {
            return makeVS(VS.findWhichVersionOfVSIsInstalled());
        } else if (typeof visualStudio === 'string') {
            return makeVS(visualStudio);
        } else {
            return visualStudio;
        }

        function makeVS(version) {
            if (version === '2017') {
                return new VS2017();
            } else {
                return new VS2015OrEarlier();
            }
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
        this.visualStudio.msBuild = this.getMSBuild();
    }

    getMSBuild() { //will get the most recent MSBuild Tools version, as this.visualStudio.msBuildVersions is ordered in descending version order
        const numOfVersions = this.visualStudio.msBuildVersions.length;
        for (let i = 0; i < numOfVersions; i++) {
            let msbuildTools = this.checkMSBuildVersion(this.visualStudio.msBuildVersions[i]);
            if (msbuildTools) {
                return msbuildTools;
            }
        }

        throw new Error('MSBuild tools not found');
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
