'use-strict';

const path = require('path');
const child_process = require('child_process');
const EOL = require('os').EOL;
const shell = require('shelljs');
const fs = require('fs');
const VS = require('./vs');

module.exports = class VS2017 extends VS {
    constructor() {
        super();
        this.vsWherePath = path.join(process.env['ProgramFiles(x86)'], '/Microsoft Visual Studio/Installer/vswhere.exe');
        if (fs.existsSync(this.vsWherePath)) {
            this.vsPath = child_process.execSync(`"${this.vsWherePath}" -latest -products * Microsoft.Component.MSBuild -property installationPath`).toString().split(EOL)[0];
        } else {
            throw new Error('vswhere.exe not found. Is Visual Studio 2017 or greater installed?');
        }

        this.msBuildVersions = this.findPossibleMSBuildVersions();
    }

    findPossibleMSBuildVersions() {
        const msBuildVersionsPath = path.join(this.vsPath, 'MSBuild');
        try {
            return shell.ls(msBuildVersionsPath)
                .filter((res) => typeof res === 'string' && /^[0-9]+.([0-9])+$/.test(res))
                .sort((v1, v2) => parseFloat(v2) - parseFloat(v1));
        } catch (e) {
            throw new Error('No versions of MSBuild found. Error: ' + e.message);
        }
    }

    getMSBuildToolsPath(version) {
        let toolsPath = null;
        // look for the specified version of msbuild
        const msBuildPath = path.join(this.vsPath, 'MSBuild', version, 'Bin/MSBuild.exe');
        toolsPath = fs.existsSync(msBuildPath) ? path.dirname(msBuildPath) : null;

        return toolsPath;
    }

    toString() {
        return '2017';
    }
};
