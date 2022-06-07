const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithHermes.sln',
  project: {
    projectFile: 'WithHermes\\WithHermes.vcxproj',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
};
