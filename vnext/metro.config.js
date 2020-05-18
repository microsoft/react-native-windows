/**
 * @format
 */
const fs = require('fs');
const path = require('path');
const rnwPath = __dirname;
const {resolve} = require('metro-resolver');

function reactNativePlatformResolver(platformImplementations) {
  return (context, _realModuleName, platform, moduleName) => {
    let backupResolveRequest = context.resolveRequest;
    delete context.resolveRequest;

    try {
      let modifiedModuleName = moduleName;
      if (platformImplementations[platform]) {
        if (moduleName === 'react-native') {
          modifiedModuleName = platformImplementations[platform];
        } else if (moduleName.startsWith('react-native/')) {
          modifiedModuleName = `${
            platformImplementations[platform]
          }/${modifiedModuleName.slice('react-native/'.length)}`;
        }
      }
      let result = resolve(context, modifiedModuleName, platform);
      return result;
    } catch (e) {
      throw e;
    } finally {
      context.resolveRequest = backupResolveRequest;
    }
  };
}

module.exports = {
  // WatchFolders is only needed due to the yarn workspace layout of node_modules, we need to watch the symlinked locations separately
  watchFolders: [
    // Include hoisted modules
    path.resolve(__dirname, '../node_modules'),
  ],

  resolver: {
    // We need a custom resolveRequest right now since our integration tests use a "windesktop" platform thats specific to integration tests.
    resolveRequest: reactNativePlatformResolver({
      windesktop: 'react-native-windows',
      windows: 'react-native-windows',
    }),
    extraNodeModules: {
      // Redirect react-native-windows to this folder
      'react-native-windows': rnwPath,
    },
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
