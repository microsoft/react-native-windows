/**
 * Part of our CI loop verifies that react-native-windows can build against react-native, as well as the microsoft fork of react-native
 * This script modifies package.json to have devDependencies that use standard react-native.
 * @tscheck
 * @format
 */
const path = require('path');
const fs = require('fs');

const pkgJsonPath = path.resolve(__dirname, '../package.json');
let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, 'utf8'));

pkgJson.devDependencies['react-native'] = pkgJson.peerDependencies[
  'react-native'
].slice(0, pkgJson.peerDependencies['react-native'].indexOf(' '));
fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2) + '\n');
