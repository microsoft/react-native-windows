module.exports = require('@rnx-kit/jest-preset')("win32", {
  verbose: true,
  snapshotResolver: './jest-snapshot-resolver.js',
  transform: {
    '^.+\\.(bmp|gif|jpg|jpeg|mp4|png|psd|svg|webp)$':
      '<rootDir>/jest/assetFileTransformer.js',
  },
  transformIgnorePatterns: ['jest-runner'],
  fakeTimers: {
    enableGlobally: true,
    legacyFakeTimers: true,
  },
  snapshotFormat: {
    escapeString: true,
    printBasicPrototype: true,
  },
  testRegex: '/__tests__/.*-test\\.js$',
  testPathIgnorePatterns: [
    // Only run the version of the tests that are part of the merged source output
    'src',
  ],
  unmockedModulePathPatterns: [
    'react',
    'Libraries/Renderer',
    'promise',
    'source-map',
    'fastpath',
    'denodeify',
  ],
  testEnvironment: 'node',
  collectCoverageFrom: ['Libraries/**/*.js'],
  coveragePathIgnorePatterns: [
    '/__tests__/',
    '/vendor/',
    '<rootDir>/Libraries/react-native/',
  ],
});
