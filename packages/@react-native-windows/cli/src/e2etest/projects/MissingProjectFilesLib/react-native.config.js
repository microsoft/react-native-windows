const projectConfig = {
  sourceDir: 'windows',
};

const expectedConfig = {
  sourceDir: 'windows',
  solutionFile: null,
  projects: [],
  nugetPackages: [],
};

module.exports = {
  dependency: {
    platforms: {
      windows: projectConfig,
    }
  },
  expectedConfig: expectedConfig,
  expectedConfigIgnoringOverride: expectedConfig,
};
