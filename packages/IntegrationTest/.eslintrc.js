module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: {tsconfigRootDir : __dirname},
  overrides: [
    // Let the runner access Jest Globals
    {
      files: ['runner/**'],
      globals: {
        "afterAll": 'readonly',
        "beforeAll": 'readonly',
        "describe": 'readonly',
        "fail": 'readonly',
        "test": 'readonly',
      }
    },
    // Jest rules get confused with chai "expect"
    {
      files: ["tests/**"],
      rules: {"jest/valid-expect": 'off'}
    },
  ],
};
