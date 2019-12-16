const path = require('path');

export default require('@office-iss/sdx-build-tools').createHaulConfig({
  isPlatformPackage: true,
  hasteAliases: {
    AsyncStorage: path.resolve(__dirname, 'src/RNTester/AsyncStorageStub')
  },
  additionalBannedPackages: ['react-native'], // react-native should have been redirected to this directory - Otherwise we'll end up with multiple copies
  webpackConfig: {
    resolve: {
      alias: {
        'react-native': path.resolve(__dirname, 'lib')
      }
    }
  },
  allowedPackageDups: ['fbjs']
});
