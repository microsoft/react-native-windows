const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCSharpLib.sln',
  project: {
    projectFile: 'SimpleCSharpLib\\SimpleCSharpLib.csproj',
    directDependency: true,
  },
};

const expectedConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCSharpLib.sln',
  projects: [{
    projectFile: 'SimpleCSharpLib\\SimpleCSharpLib.csproj',
    directDependency: true,
    projectName: 'SimpleCSharpLib',
    projectLang: 'cs',
    projectGuid: '{416476D5-974A-4EE2-8145-4E331297247E}',
    cppHeaders: ["winrt/SimpleCSharpLib.h"],
    cppPackageProviders: ["SimpleCSharpLib::ReactPackageProvider"],
    csNamespaces: ["SimpleCSharpLib"],
    csPackageProviders: ["SimpleCSharpLib.ReactPackageProvider"],
  }],
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
