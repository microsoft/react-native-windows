// @ts-check

module.exports = {
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
  };
