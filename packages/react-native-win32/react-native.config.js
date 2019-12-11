// @ts-check

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
    reactNativePath: '.',
  };
