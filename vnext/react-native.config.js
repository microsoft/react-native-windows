// @ts-check

module.exports = {
    // **** This section defined commands and options on how to provide the windows platform to external applications
    commands: [
      require('./local-cli/runWindows/runWindows'),
    ],
    platforms: {
      windows: {
        linkConfig: () => null,
        projectConfig: (projectRoot, projectParams) => null,
        dependencyConfig: (projectRoot, dependencyParams) => null,
      },
    },

    // *****
    // This is only used when building bundles within react-native-windows.
    reactNativePath: '.',
  };
