'use-strict';

const path = require('path');

function getNuGetPath() {
    return 'node_modules/nuget-exe/.nuget/nuget.exe';
}

module.exports = {getNuGetPath};