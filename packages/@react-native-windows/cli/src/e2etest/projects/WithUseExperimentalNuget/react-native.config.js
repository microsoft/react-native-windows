const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithUseExperimentalNuget.sln',
  project: {
    projectFile: 'WithUseExperimentalNuget\\WithUseExperimentalNuget.vcxproj',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
};
