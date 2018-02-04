'use strict';

module.exports = class MSBuildTools {
    constructor(version, localPath) {
        this.version = version;
        this.path = localPath;
    }
};
