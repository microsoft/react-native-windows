const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCSharpLib.sln',
  project: {
    projectFile: 'SimpleCSharpLib\\SimpleCSharpLib.csproj',
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
