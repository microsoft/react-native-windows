// @ts-check
require('source-map-support').install();

const cli = require('@react-native-windows/cli');

module.exports = {
    // **** This section defined commands and options on how to provide the Windows platform to external applications
    commands: cli.commands,
    platforms: {
      windows: {
        linkConfig: () => null,
        projectConfig: cli.projectConfig,
        dependencyConfig: cli.dependencyConfig,
        npmPackageName: 'react-native-windows',
      },
    },
  };
