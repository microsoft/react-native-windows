// @ts-check
const execSync = require('child_process').execSync;
const path = require('path');
const rootPath = execSync('git rev-parse --show-toplevel');

// The default value of our devDependencies assumes that the react-native package is available on a NPM feed
// For users outside of sdx-platform, we should redirect the dev dependency to a tar from github
if (path.normalize(rootPath.toString().trim()) === path.resolve(__dirname, '../..')) {
    require('./replaceRNDependency');
}