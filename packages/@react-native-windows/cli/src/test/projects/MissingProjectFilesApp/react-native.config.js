const projectConfig = {
  sourceDir: 'windows',
};

const expectedConfig = {
  sourceDir: 'windows',
  solutionFile: 'Error: Solution file is required but not specified in react-native.config.',
  project: {
    projectFile: 'Error: Project is required but not specified in react-native.config.',
  }
};

const expectedConfigIgnoringOverride = {
  sourceDir: 'windows',
  solutionFile: 'Error: No app solution file found, please specify in react-native.config.',
  project: {
    projectFile: 'Error: No app project file found, please specify in react-native.config.',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
  expectedConfig: expectedConfig,
  expectedConfigIgnoringOverride: expectedConfigIgnoringOverride,
};
