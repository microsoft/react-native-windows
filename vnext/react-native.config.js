// @ts-check
const projectConfig = require('./local-cli/config/projectConfig');
const dependencyConfig = require('./local-cli/config/dependencyConfig');

module.exports = {
    // **** This section defined commands and options on how to provide the windows platform to external applications
    commands: [
      require('./local-cli/runWindows/runWindows'),
      require('./local-cli/runWindows/utils/autolink'),
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
