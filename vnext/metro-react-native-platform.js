/**
 * @format
 */
const {resolve} = require('metro-resolver');

/**
 * platformImplementations is a map of platform to npm package that implements that platform
 *
 * Ex:
 * {
 *    windows: 'react-native-windows'
 *    macos: 'react-native-macos'
 * }
 */
function reactNativePlatformResolver(platformImplementations) {
  return (context, _realModuleName, platform, moduleName) => {
    let backupResolveRequest = context.resolveRequest;
    delete context.resolveRequest;

    try {
      let modifiedModuleName = moduleName;
      if (platformImplementations[platform]) {
        if (moduleName === 'react-native') {
          modifiedModuleName = platformImplementations[platform];
        } else if (moduleName.startsWith('react-native/')) {
          modifiedModuleName = `${
            platformImplementations[platform]
          }/${modifiedModuleName.slice('react-native/'.length)}`;
        }
      }
      let result = resolve(context, modifiedModuleName, platform);
      context.resolveRequest = backupResolveRequest;
      return result;
    } catch (e) {
      context.resolveRequest = backupResolveRequest;
      throw e;
    }
  };
}

/**
 * The CLI will get a more complete implementation of this in https://github.com/react-native-community/cli/pull/1115
 * but until then, use a solution that supports having react-native-windows and/or react-native-macos
 */
const getModulesRunBeforeMainModule = () => {
  const options = {
    paths: [process.cwd()],
  };
  const modules = [
    require.resolve('react-native/Libraries/Core/InitializeCore', options),
  ];

  try {
    modules.push(
      require.resolve(
        'react-native-windows/Libraries/Core/InitializeCore',
        options,
      ),
    );
  } catch {}

  try {
    modules.push(
      require.resolve(
        'react-native-macos/Libraries/Core/InitializeCore',
        options,
      ),
    );
  } catch {}

  return modules;
};

module.exports = {
  getModulesRunBeforeMainModule,
  reactNativePlatformResolver,
};
