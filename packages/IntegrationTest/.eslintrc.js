module.exports = {
  extends: ['@rnw-scripts'],
  overrides: [
    // Let the runner access Jest Globals
    {
      files: ['runner/**'],
      globals: {
        "afterAll": 'readonly',
        "beforeAll": 'readonly',
        "fail": 'readonly',
        "test": 'readonly',
      }
    }
  ],
  parserOptions: {tsconfigRootDir : __dirname},
};
