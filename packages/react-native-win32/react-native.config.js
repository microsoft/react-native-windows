// @ts-check

const path = require('path');
module.exports = {
    platforms: {
      win32: {
        linkConfig: () => null,
        projectConfig: (projectRoot, projectParams) => null,
        dependencyConfig: (projectRoot, dependencyParams) => null,
      },
    },

    // *****
    // This is only used when building bundles within react-native-win32.
    reactNativePath: path.resolve(__dirname, './react-native-installation'),
  };
