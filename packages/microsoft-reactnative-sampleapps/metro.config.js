/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */
const path = require('path');
const blacklist = require('metro-config/src/defaults/blacklist');
const rnwPath = path.resolve(__dirname, '../../vnext');

module.exports = {
  // WatchFolders is only needed due to the yarn workspace layout of node_modules, we need to watch the symlinked locations separately
  watchFolders: [
    // Include hoisted modules
    path.resolve(__dirname, '../..', 'node_modules'),
    // Include react-native-windows
    rnwPath,
  ],

  resolver: {
    extraNodeModules: {
      // Redirect metro to rnwPath instead of node_modules/react-native-windows, since metro doesn't like symlinks
      'react-native-windows': rnwPath,
    },
    blacklistRE: blacklist([
      // Avoid error EBUSY: resource busy or locked, open 'D:\a\1\s\packages\E2ETest\msbuild.ProjectImports.zip' in pipeline
      /.*\.ProjectImports\.zip/,
      // This stops "react-native run-windows" from causing the metro server to crash if its already running
      new RegExp(
        `${path.resolve(__dirname, 'windows').replace(/[/\\]/g, '/')}.*`,
      ),
    ]),
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
