module.exports = {
  extends: ['@rnw-scripts'],
  overrides: [
    // Jest rules get confused with chai "expect"
    {
      files: ["tests/**"],
      rules: {"jest/valid-expect": 'off'}
    },
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
};
