const config = require('@rnx-kit/jest-preset')('windows');

config.moduleNameMapper = {
  '^react-native/react-private-interface$': require.resolve(
    'react-native-windows/src/react-private-interface.js',
  ),
  '^react-native/setup-env$': require.resolve(
    'react-native-windows/src/setup-env.js',
  ),
  ...config.moduleNameMapper,
};

module.exports = config;
