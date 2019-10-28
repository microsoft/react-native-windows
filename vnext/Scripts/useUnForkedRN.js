/**
 * Part of our CI loop verifies that react-native-windows can build against react-native, as well as the microsoft fork of react-native
 * This script modifies the yarn workspace resolutions to force the react-native version to be the facebook version of react-native
 * @tscheck
 * @format
 */

const path = require('path');
const fs = require('fs');

const pkgJson = require(path.resolve(__dirname, '../package.json'));

const rnVersion = pkgJson.peerDependencies['react-native'].split(' ')[0];

const rootPkgJsonPath = path.resolve(__dirname, '../../package.json');
const rootPkgJson = require(rootPkgJsonPath);

if (!rootPkgJson.resolutions) {
  rootPkgJson.resolutions = {};
}
rootPkgJson.resolutions['**/react-native'] = rnVersion;

fs.writeFileSync(rootPkgJsonPath, JSON.stringify(rootPkgJson, null, 2) + '\n');
