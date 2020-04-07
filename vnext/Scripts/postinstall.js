/**
 * @format
 */
// @ts-check

const path = require('path');
const installationPath = path.resolve(require.resolve('react-native-windows/package.json'), '../../../react-native-installation');
require('./installRNW').installRNW(installationPath);