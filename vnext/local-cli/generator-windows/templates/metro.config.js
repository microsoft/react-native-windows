/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */
const path = require('path');
const blacklist = require('metro-config/src/defaults/blacklist');

const {
  getModulesRunBeforeMainModule,
  reactNativePlatformResolver,
} = require('react-native-windows/metro-react-native-platform');

module.exports = {
  resolver: {
    resolveRequest: reactNativePlatformResolver({
      windows: 'react-native-windows',
    }),
    blacklistRE: blacklist([
      // This stops "react-native run-windows" from causing the metro server to crash if its already running
      new RegExp(
        `${path.resolve(__dirname, 'windows').replace(/[/\\]/g, '/')}.*`,
      ),
      // This prevents "react-native run-windows" from hitting: EBUSY: resource busy or locked, open msbuild.ProjectImports.zip
      new RegExp(
        `${path
          .resolve(__dirname, 'msbuild.ProjectImports.zip')
          .replace(/[/\\]/g, '/')}.*`,
      ),
    ]),
  },
  serializer: {
    getModulesRunBeforeMainModule,
  },
  transformer: {
    // The cli defaults this to a full path to react-native, which bypasses the reactNativePlatformResolver above
    // Hopefully we can fix the default in the future
    assetRegistryPath: 'react-native/Libraries/Image/AssetRegistry',
    getTransformOptions: async () => ({
      transform: {
        experimentalImportSupport: false,
        inlineRequires: false,
      },
    }),
  },
};
