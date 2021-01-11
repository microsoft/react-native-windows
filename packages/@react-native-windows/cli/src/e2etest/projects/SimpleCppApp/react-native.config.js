const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCppApp.sln',
  project: {
    projectFile: 'SimpleCppApp\\SimpleCppApp.vcxproj',
  },
};

const expectedConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCppApp.sln',
  project: {
    projectFile: 'SimpleCppApp\\SimpleCppApp.vcxproj',
    projectName: 'SimpleCppApp',
    projectLang: 'cpp',
    projectGuid: '{416476d5-974a-4ee2-8145-4e331297247e}',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
  expectedConfig: expectedConfig,
  expectedConfigIgnoringOverride: expectedConfig,
};
