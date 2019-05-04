const revokePatch = require('./patches/revokePatch');

const makeProjectPatch = require('./patches/makeProjectPatch');
const makeSolutionPatch = require('./patches/makeSolutionPatch');
const makeConfigurationPatch = require('./patches/makeConfigurationPatch');
const makeUsingPatch = require('./patches/makeUsingPatch');
const makePackagePatch = require('./patches/makePackagePatch');

module.exports = function unregisterNativeWindowsModule(
  name,
  windowsConfig,
  projectConfig
) {
  revokePatch(projectConfig.projectPath, makeProjectPatch(windowsConfig));
  revokePatch(projectConfig.solutionPath, makeSolutionPatch(windowsConfig));
  revokePatch(projectConfig.solutionPath, makeConfigurationPatch(windowsConfig));

  revokePatch(
    projectConfig.mainFilePath,
    makePackagePatch(windowsConfig.packageInstance, {}, name)
  );

  revokePatch(
    projectConfig.mainFilePath,
    makeUsingPatch(windowsConfig.packageUsingPath)
  );
};
