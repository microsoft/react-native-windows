'use-strict';

function getConfiguration(release) {
    return release ? 'Release' : 'Debug';
}

function getVerboseCLP(verbose) {
    return verbose ? 'normal' : 'quiet';
}

module.exports = {getConfiguration, getVerboseCLP};
