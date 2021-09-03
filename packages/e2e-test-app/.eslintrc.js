module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: { tsconfigRootDir: __dirname },
  globals: {
    expect: 'readonly',
    fail: 'readonly',
    AutomationClient: 'readonly',
  },
};
