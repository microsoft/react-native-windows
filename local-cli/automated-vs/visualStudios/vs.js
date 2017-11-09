/*
 * Some descendant of this class must have:
 * getMSBuildToolsPath
 * toString
 */

'use-strict';

module.exports = class VS {
    constructor() {
        this.msBuild = null;
    }

    getVSVersion() {
        return this.toString();
    }
};
