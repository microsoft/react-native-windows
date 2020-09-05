/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const path = require('path');

module.exports = {
  extends: require.resolve('@rnw-scripts/eslint-root-config'),
  overrides: [
    {
      files: ['**/*(!.d).ts', '*.tsx'],
      parser: '@typescript-eslint/parser',
      plugins: ['@typescript-eslint/eslint-plugin'],
      parserOptions: {
        project: path.join(process.cwd(), 'tsconfig.json'),
      },
      rules: {
        '@typescript-eslint/no-floating-promises': [
          'error',
          {ignoreIIFE: true},
        ],
      },
    },
  ],
};
