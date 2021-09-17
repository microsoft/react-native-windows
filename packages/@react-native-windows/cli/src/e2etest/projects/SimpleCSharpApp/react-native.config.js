const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCSharpApp.sln',
  project: {
    projectFile: 'SimpleCSharpApp\\SimpleCSharpApp.csproj',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
  dependency: {
    platforms: {
      windows: projectConfig,
    }
  },
};
