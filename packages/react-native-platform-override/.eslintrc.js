/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

module.exports = {
  extends: ['@rnw-scripts'],
  rules: {
    'sort-imports': 'error',
  },
  ignorePatterns: ['src/e2etest/collateral/**'],
  parserOptions: {tsconfigRootDir: __dirname},
};
