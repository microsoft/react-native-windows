/**
 * @format
 */
// @ts-check

const path = require('path');
const reactNativeWindowsPath = path.resolve(
  require.resolve('react-native-windows/package.json'),
  '..',
);
const installationPath = path.resolve(
  reactNativeWindowsPath,
  '../../react-native-installation',
);
require('./installRNW').installRNW(reactNativeWindowsPath, installationPath);
