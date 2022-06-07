const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithExperimentalNuGet.sln',
  project: {
    projectFile: 'WithExperimentalNuGet\\WithExperimentalNuGet.vcxproj',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
};
