// @ts-check

module.exports = {
    platforms: {
      win32: {
        linkConfig: () => null,
        projectConfig: (projectRoot, projectParams) => null,
        dependencyConfig: (projectRoot, dependencyParams) => null,
        npmPackageName: '@office-iss/react-native-win32',
        // Enable once CLI config supports it - https://github.com/react-native-community/cli/pull/2002
        // saveAssetsPlugin: '@office-iss/react-native-win32/saveAssetPlugin'
      },
    },
  };
