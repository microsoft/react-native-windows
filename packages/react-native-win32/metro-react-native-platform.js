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

module.exports = {reactNativePlatformResolver};
