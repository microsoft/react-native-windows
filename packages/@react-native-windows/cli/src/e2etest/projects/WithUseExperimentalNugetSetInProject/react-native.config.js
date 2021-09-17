const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithUseExperimentalNugetSetInProject.sln',
  project: {
    projectFile: 'WithUseExperimentalNugetSetInProject\\WithUseExperimentalNugetSetInProject.vcxproj',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
};
