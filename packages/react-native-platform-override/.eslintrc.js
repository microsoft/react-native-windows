/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

module.exports = {
  extends: ['@rnw-scripts'],
  ignorePatterns: ['src/e2etest/collateral/**'],
  parserOptions: {tsconfigRootDir: __dirname},
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
