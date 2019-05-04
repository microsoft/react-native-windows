const applyPatch = require('./patches/applyPatch');

const makeProjectPatch = require('./patches/makeProjectPatch');
const makeSolutionPatch = require('./patches/makeSolutionPatch');
const makeConfigurationPatch = require('./patches/makeConfigurationPatch');
const makeUsingPatch = require('./patches/makeUsingPatch');
const makePackagePatch = require('./patches/makePackagePatch');

module.exports = function registerNativeWindowsModule(
  name,
  windowsConfig,
  params,
  projectConfig
) {
  applyPatch(projectConfig.projectPath, makeProjectPatch(windowsConfig), true);
  applyPatch(projectConfig.solutionPath, makeSolutionPatch(windowsConfig), true);
  applyPatch(projectConfig.solutionPath, makeConfigurationPatch(windowsConfig), true);

  applyPatch(
    projectConfig.mainFilePath,
    makePackagePatch(windowsConfig.packageInstance, params, name)
  );

  applyPatch(
    projectConfig.mainFilePath,
    makeUsingPatch(windowsConfig.packageUsingPath)
  );
};
