module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: { tsconfigRootDir: __dirname },
  globals: {
    expect: 'readonly',
    fail: 'readonly',
    AutomationClient: 'readonly',
  },
  overrides: [
    {
      files: ['*.ts'],
      // Also not clean
      rules: {
        '@typescript-eslint/no-unused-vars': 'off',
      },
    },
  ],
};
