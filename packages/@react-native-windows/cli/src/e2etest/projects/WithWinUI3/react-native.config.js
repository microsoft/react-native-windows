const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithWinUI3.sln',
  project: {
    projectFile: 'WithWinUI3\\WithWinUI3.vcxproj',
  },
  useWinUI3: true,
};

const expectedConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithWinUI3.sln',
  project: {
    projectFile: 'WithWinUI3\\WithWinUI3.vcxproj',
    projectName: 'WithWinUI3',
    projectLang: 'cpp',
    projectGuid: '{416476d5-974a-4ee2-8145-4e331297247e}',
  },
  useWinUI3: true,
};

const { useWinUI3, ...expectedConfigIgnoringOverride } = expectedConfig;

module.exports = {
  project: {
    windows: projectConfig,
  },
  expectedConfig: expectedConfig,
  expectedConfigIgnoringOverride: expectedConfigIgnoringOverride,
};
