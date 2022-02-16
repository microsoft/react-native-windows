const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCppLib.sln',
  project: {
    projectFile: 'SimpleCppLib\\SimpleCppLib.vcxproj',
    directDependency: true,
  },
};

module.exports = {
  dependency: {
    platforms: {
      windows: projectConfig,
    }
  },
};
