// @ts-check

module.exports = {
    platforms: {
      win32: {
        linkConfig: () => null,
        projectConfig: (projectRoot, projectParams) => null,
        dependencyConfig: (projectRoot, dependencyParams) => null,
        npmPackageName: '@office-iss/react-native-win32',
      },
    },
  };
