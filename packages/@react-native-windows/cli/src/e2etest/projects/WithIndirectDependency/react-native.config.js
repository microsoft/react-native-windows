const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'WithIndirectDependency.sln',
  project: {
    projectFile: 'WithIndirectDependency\\WithIndirectDependency.vcxproj',
  },
  useWinUI3: false,
};

module.exports = {
  project: {
    windows: projectConfig,
  },
  dependency: {
    platforms: {
      windows: {
        sourceDir: 'windows',
        solutionFile: 'WithIndirectDependency.sln',
        projects: [
          {
            projectFile: 'WithIndirectDependency\\WithIndirectDependency.vcxproj',
            directDependency: true,
          },
          {
            projectFile: 'IndirectDependency\\IndirectDependency.vcxproj',
            directDependency: false,
          }
        ],
      },
    },
  },
};
