const config = require('@rnx-kit/jest-preset')('windows');

config.moduleNameMapper = {
  '^react-native/react-private-interface$': require.resolve(
    'react-native-windows/react-private-interface.js',
  ),
  '^react-native/setup-env$': require.resolve(
    'react-native-windows/setup-env.js',
  ),
  ...config.moduleNameMapper,
};

module.exports = config;
