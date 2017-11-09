'use strict';

const MSBUILD_VERSIONS = ['15.0', '14.0', '12.0', '4.0'];

class MSBuildTools {
    constructor(version, localPath) {
        this.version = version;
        this.path = localPath;
    }
}

module.exports = {MSBuildTools, MSBUILD_VERSIONS};
