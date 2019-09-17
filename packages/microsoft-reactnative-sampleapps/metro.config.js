/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */
const fs = require('fs');
const path = require('path');
const blacklist = require('metro-config/src/defaults/blacklist');

const rnPath = fs.realpathSync(
  path.resolve(require.resolve('react-native/package.json'), '..'),
);
const rnwPath = path.resolve(__dirname, '../../vnext');
const rnwePath = path.resolve(__dirname, '../react-native-windows-extended');

module.exports = {
  // WatchFolders is only needed due to the yarn workspace layout of node_modules, we need to watch the symlinked locations separately
  watchFolders: [
    // Include hoisted modules
    path.resolve(__dirname, '../..', 'node_modules'),
    // Include react-native-windows
    rnwPath,
    // Include react-native-windows-extended
    rnwePath,
  ],

  resolver: {
    extraNodeModules: {
      // Redirect metro to rnwPath instead of node_modules/react-native-windows, since metro doesn't like symlinks
      'react-native': rnwPath,
      'react-native-windows': rnwPath,
      'react-native-windows-extended': rnwePath,
    },
    // Include the macos platform in addition to the defaults because the fork includes macos, but doesn't declare it
    platforms: ['ios', 'android', 'windesktop', 'windows', 'web', 'macos'],
    providesModuleNodeModules: ['react-native', 'react-native-windows'],
    // Since there are multiple copies of react-native, we need to ensure that metro only sees one of them
    // This should go away after RN 0.60 when haste is removed
    blacklistRE: blacklist([
      new RegExp(`${path.resolve(rnPath).replace(/[/\\\\]/g, '[/\\\\]')}.*`),
      new RegExp(
        `${path
          .resolve(rnwPath, 'node_modules/react-native')
          .replace(/[/\\\\]/g, '[/\\\\]')}.*`,
      ),
      new RegExp(
        `${path
          .resolve(rnwePath, 'node_modules/react-native')
          .replace(/[/\\\\]/g, '[/\\\\]')}.*`,
      ),
      new RegExp(
        `${path
          .resolve(
            require.resolve('@react-native-community/cli/package.json'),
            '../node_modules/react-native',
          )
          .replace(/[/\\\\]/g, '[/\\\\]')}.*`,
      ),
      // This stops "react-native run-windows" from causing the metro server to crash if its already running
      new RegExp(
        `${path
          .resolve(__dirname, 'windows')
          .replace(/[/\\\\]/g, '[/\\\\]')}.*`,
      ),
    ]),
    hasteImplModulePath: path.resolve(__dirname, 'hasteImpl.js'),
  },
  transformer: {
    getTransformOptions: async () => ({
      transform: {
        experimentalImportSupport: false,
        inlineRequires: false,
      },
    }),
  },
};
