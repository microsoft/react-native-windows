module.exports = function makeSolutionPatch(windowsConfig) {

  const configs = {
    'Debug': 'Debug',
    'DebugBundle': 'Debug',
    'Release': 'Release',
    'ReleaseBundle': 'Release'
  };

  const platforms = [ 'ARM', 'x64', 'x86'];

  const entries = [ 'ActiveCfg', 'Build.0' ];

  let solutionConfiguration = '';
  for (var appConfig in configs) {
    for (var platform of platforms) {
      for (var entry of entries) {
        let projectConfig = configs[appConfig];
        solutionConfiguration += `        {${windowsConfig.projectGUID.toUpperCase()}}.${appConfig}|${platform}.${entry} = ${projectConfig}|${platform}\r\n`;
      }
    }
  }

  /* eslint-disable no-control-regex */
  return {
    pattern: new RegExp('[^\S\r\n]+{C7673AD5-E3AA-468C-A5FD-FA38154E205C}.Debug'),
    patch: solutionConfiguration,
    unpatch: new RegExp(`.*{${windowsConfig.projectGUID.toUpperCase()}}.*[\r\n]*`, 'gi'),
  };
};
