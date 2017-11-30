'use-strict';

const child_process = require('child_process');
const path = require('path');
const VS = require('./vs');

module.exports = class VS2015OrEarlier extends VS {
    constructor() {
        super();
        this.msBuildRegPath = 'HKLM\\SOFTWARE\\Microsoft\\MSBuild\\ToolsVersions';
        this.msBuildVersions = this.findPossibleMSBuildVersions();
    }

    findPossibleMSBuildVersions() {
        const query = `REG QUERY ${this.msBuildRegPath}`;
        try {
            return child_process.execSync(query).toString().split('\r\n')
                .filter((versionPath) =>  versionPath !== '')
                .map((versionPath) => versionPath.match(/\\[0-9]+.([0-9])+$/)[0].slice(1))
                .sort((v1, v2) => parseFloat(v2) - parseFloat(v1));
        } catch (e) {
            throw new Error('No versions of MSBuild found. Error: ' + e.message);
        }
    }

    getMSBuildToolsPath(version) {
        let toolsPath = null;
        const query = `REG QUERY ${this.msBuildRegPath}\\${version} /s /v MSBuildToolsPath`;
        // Try to get the MSBuild path using registry
        try {
            const output = child_process.execSync(query).toString();
            let toolsPathOutput = /MSBuildToolsPath\s+REG_SZ\s+(.*)/i.exec(output);
            if (toolsPathOutput) {
                toolsPathOutput = toolsPathOutput[1];
                // Win10 on .NET Native uses x86 arch compiler, if using x64 Node, use x86 tools
                if ((version === '15.0' || version === '14.0' && toolsPathOutput.indexOf('amd64') > -1)) {
                    toolsPathOutput = path.resolve(toolsPathOutput, '..');
                }
                toolsPath = toolsPathOutput;
            }
        } catch (e) {
            toolsPath = null;
        }

        return toolsPath;
    }

    toString() {
        return '2015OrEarlier';
    }
};
