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

const expectedConfigIgnoringOverride = {
  sourceDir: 'windows',
  solutionFile: 'WithWinUI3.sln',
  project: {
    projectFile: 'WithWinUI3\\WithWinUI3.vcxproj',
    projectName: 'WithWinUI3',
    projectLang: 'cpp',
    projectGuid: '{416476d5-974a-4ee2-8145-4e331297247e}',
  },
  useWinUI3: false,
};

module.exports = {
  project: {
    windows: projectConfig,
  },
  expectedConfig: expectedConfig,
  expectedConfigIgnoringOverride: expectedConfigIgnoringOverride,
  expectedPackagesConfig: `<?xml version="1.0" encoding="utf-8"?>
<packages>
  <package id="Microsoft.ReactNative.Cxx" version="1.0.0" targetFramework="native"/>
  <package id="Microsoft.WinUI" version="3.0.0-preview3.201113.0" targetFramework="native"/>
</packages>
`,
  expectedBuildFlags: `<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">

  <!-- Flags can be added here to affect the compilation of Microsoft.ReactNative -->
  <PropertyGroup Label="Microsoft.ReactNative Build Flags">
    <UseWinUI3>true</UseWinUI3>
    <UseHermes>false</UseHermes>
  </PropertyGroup>

</Project>`,
};
