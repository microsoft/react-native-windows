/**
 * @format
 */
const fs = require('fs');
const path = require('path');
const blacklist = require('metro-config/src/defaults/blacklist');

const rnPath = fs.realpathSync(
  fs.realpathSync(
    path.resolve(require.resolve('react-native/package.json'), '..'),
  ),
);
const rnwPath = __dirname;

module.exports = {
  // WatchFolders is only needed due to the yarn workspace layout of node_modules, we need to watch the symlinked locations separately
  watchFolders: [
    // Include hoisted modules
    path.resolve(__dirname, '../node_modules'),
  ],

  resolver: {
    extraNodeModules: {
      // Redirect react-native and react-native-windows to this folder
      'react-native': rnwPath,
      'react-native-windows': rnwPath,
    },
    // Include the macos platform in addition to the defaults because the fork includes macos, but doesn't declare it
    platforms: ['ios', 'android', 'windesktop', 'windows', 'web', 'macos'],
    providesModuleNodeModules: ['react-native-windows'],
    // Since there are multiple copies of react-native, we need to ensure that metro only sees one of them
    // This should go away after RN 0.61 when haste is removed
    blacklistRE: blacklist([
      new RegExp(`${path.resolve(rnPath).replace(/[/\\\\]/g, '[/\\\\]')}.*`),
      new RegExp(
        `${path
          .resolve(
            require.resolve('@react-native-community/cli/package.json'),
            '../node_modules/react-native',
          )
          .replace(/[/\\\\]/g, '[/\\\\]')}.*`,
      ),
    ]),
    hasteImplModulePath: path.resolve(__dirname, 'jest/hasteImpl.js'),
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

// Check that we have built our JS files before running the bundler, otherwise we'll get a harder to diagnose "Unable to resolve module" error
if (
  !fs.existsSync(
    path.resolve(
      __dirname,
      'Libraries/Components/AccessibilityInfo/AccessibilityInfo.windows.js',
    ),
  )
) {
  throw new Error(
    '[31m\nThis package must be built before running the bundler.  Did you mean to run "[39m[33myarn build[39m[31m" first?[39m\n',
  );
}
