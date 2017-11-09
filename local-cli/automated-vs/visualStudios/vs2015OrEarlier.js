'use-strict';

const child_process = require('child_process');
const path = require('path');
const VS = require('./vs');

module.exports = class VS2015OrEarlier extends VS {

    getMSBuildToolsPath(version) {
        let toolsPath = null;
        const query = `reg query HKLM\\SOFTWARE\\Microsoft\\MSBuild\\ToolsVersions\\${version} /s /v MSBuildToolsPath`;
        // Try to get the MSBuild path using registry
        try {
            const output = child_process.execSync(query).toString();
            let toolsPathOutput = /MSBuildToolsPath\s+REG_SZ\s+(.*)/i.exec(output);
            if (toolsPathOutput) {
                toolsPathOutput = toolsPathOutput[1];
                // Win10 on .NET Native uses x86 arch compiler, if using x64 Node, use x86 tools
                if ((version === '15.0' || version === '14.0' && toolsPath.indexOf('amd64') > -1)) {
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
