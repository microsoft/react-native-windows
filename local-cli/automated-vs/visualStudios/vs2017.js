'use-strict';

const path = require('path');
const child_process = require('child_process');
const EOL = require('os').EOL;
const fs = require('fs');
const VS = require('./vs');

module.exports = class VS2017 extends VS {

    getMSBuildToolsPath(version) {
        let toolsPath = null;
        const vsWherePath = path.join(process.env['ProgramFiles(x86)'], '/Microsoft Visual Studio/Installer/vswhere.exe');
        const vsPath = child_process.execSync(`"${vsWherePath}" -latest -products * Microsoft.Component.MSBuild -property installationPath`).toString().split(EOL)[0];
        // look for the specified version of msbuild
        const msBuildPath = path.join(vsPath, 'MSBuild', version, 'Bin/MSBuild.exe');
        toolsPath = fs.existsSync(msBuildPath) ? path.dirname(msBuildPath) : null;

        return toolsPath;
    }

    toString() {
        return '2017';
    }
}