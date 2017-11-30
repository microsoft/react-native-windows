/*
 * Some descendant of this class must have:
 * getMSBuildToolsPath
 * findPossibleMSBuildVersions
 * toString
 */

'use-strict';

const fs = require('fs');
const path = require('path');

module.exports = class VS {

    static findWhichVersionOfVSIsInstalled() {
        //if vswhere.exe exists, using Visual Studio 2017
        const vsWherePath = path.join(process.env['ProgramFiles(x86)'], '/Microsoft Visual Studio/Installer/vswhere.exe');
        if (fs.existsSync(vsWherePath)) {
            return '2017';
        } else {
            return '2015OrEarlier';
        }
    }

    getVSVersion() {
        return this.toString();
    }
};
