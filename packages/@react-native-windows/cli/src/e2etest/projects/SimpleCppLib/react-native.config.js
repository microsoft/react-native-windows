const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCppLib.sln',
  project: {
    projectFile: 'SimpleCppLib\\SimpleCppLib.vcxproj',
    directDependency: true,
  },
};

const expectedConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCppLib.sln',
  projects: [{
    projectFile: 'SimpleCppLib\\SimpleCppLib.vcxproj',
    directDependency: true,
    projectName: 'SimpleCppLib',
    projectLang: 'cpp',
    projectGuid: '{416476d5-974a-4ee2-8145-4e331297247e}',
    cppHeaders: ["winrt/SimpleCppLib.h"],
    cppPackageProviders: ["SimpleCppLib::ReactPackageProvider"],
    csNamespaces: ["SimpleCppLib"],
    csPackageProviders: ["SimpleCppLib.ReactPackageProvider"],
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
