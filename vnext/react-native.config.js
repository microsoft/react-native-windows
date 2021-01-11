// @ts-check
const cli = require('@react-native-windows/cli');

module.exports = {
    // **** This section defined commands and options on how to provide the Windows platform to external applications
    commands: cli.commands,
    platforms: {
      windows: {
        linkConfig: () => null,
<<<<<<< HEAD
        projectConfig: projectConfig.projectConfigWindows,
        dependencyConfig: dependencyConfig.dependencyConfigWindows,
        npmPackageName: 'react-native-windows',
||||||| 811c767bf
        projectConfig: projectConfig.projectConfigWindows,
        dependencyConfig: dependencyConfig.dependencyConfigWindows,
=======
        projectConfig: cli.projectConfig,
        dependencyConfig: cli.dependencyConfig,
        npmPackageName: 'react-native-windows',
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa
      },
    },
  };
