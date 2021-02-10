const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithWinUI3.sln',
  project: {
    projectFile: 'WithWinUI3\\WithWinUI3.vcxproj',
  },
  useWinUI3: true,
};

module.exports = {
  project: {
    windows: projectConfig,
  },
};
