// @ts-check
const projectConfig = require('./local-cli/lib-commonjs/config/projectConfig');
const dependencyConfig = require('./local-cli/lib-commonjs/config/dependencyConfig');

module.exports = {
    // **** This section defined commands and options on how to provide the Windows platform to external applications
    commands: [
      require('./local-cli/lib-commonjs/runWindows/runWindows'),
      require('./local-cli/lib-commonjs/runWindows/utils/autolink'),
    ],
    platforms: {
      windows: {
        linkConfig: () => null,
        projectConfig: projectConfig.projectConfigWindows,
        dependencyConfig: dependencyConfig.dependencyConfigWindows,
        npmPackageName: 'react-native-windows',
      },
    },
  };
