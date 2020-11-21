/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

module.exports = {
  extends: ['@react-native-community', 'prettier'],
  rules: {
    'jest/no-disabled-tests': 'off',
    'react-native/no-inline-styles': 'off',
    'no-void': 'off',
  },
  env: {
    node: true,
  },
  ignorePatterns: ['/lib/**', '/lib-commonjs/**'],
  overrides: [
    {
      files: ['*.ts', '*.tsx'],
      excludedFiles: ['*.d.ts'],
      parser: '@typescript-eslint/parser',
      plugins: ['@typescript-eslint/eslint-plugin'],
      parserOptions: {
        project: './tsconfig.json',
      },
      rules: {
        '@typescript-eslint/await-thenable': 'error',
        '@typescript-eslint/ban-tslint-comment': 'error',
        '@typescript-eslint/ban-types': [
          'error',
          {
            'extendDefaults': true,
            'types': {
              // See https://github.com/typescript-eslint/typescript-eslint/issues/2063
              '{}': false
            }
          }
        ],
         // Avoid some naming convention checks for JSX files where function components are capitalized
         '@typescript-eslint/naming-convention': [
          'error',
          { "selector": "class", "format": ["PascalCase"], leadingUnderscore: 'allow' },
          { "selector": "interface", "format": ["PascalCase"], leadingUnderscore: 'allow' },
          { "selector": "typeAlias", "format": ["PascalCase"], leadingUnderscore: 'allow' },
        ],
        '@typescript-eslint/no-confusing-non-null-assertion': 'error',
        '@typescript-eslint/no-extra-non-null-assertion': 'error',
        '@typescript-eslint/no-floating-promises': [
          'error',
          {ignoreIIFE: true},
        ],
        '@typescript-eslint/no-for-in-array': 'error',
        '@typescript-eslint/no-misused-new': 'error',
        '@typescript-eslint/no-misused-promises': 'error',
        '@typescript-eslint/no-namespace': 'error',
        '@typescript-eslint/no-this-alias': 'error',
        '@typescript-eslint/no-unnecessary-boolean-literal-compare': 'error',
        '@typescript-eslint/no-unnecessary-condition': ['error', {
          allowConstantLoopConditions: true
        }],
        '@typescript-eslint/prefer-for-of': 'error',
        '@typescript-eslint/prefer-includes': 'error',
        '@typescript-eslint/prefer-readonly': 'error',
        '@typescript-eslint/prefer-string-starts-ends-with': 'error',
        '@typescript-eslint/switch-exhaustiveness-check': 'error',
        'no-restricted-syntax': ['error', {
          selector: 'TSEnumDeclaration',
          message: `Prefer union types (e.g. type Foo = 'bar' | 'baz') to TypeScript enums`
        }],
        'block-scoped-var': 'error',
        'complexity': 'warn',
        'guard-for-in': 'error',
        'no-constructor-return': 'error',
        'no-useless-concat': 'error',
        'no-var': 'error',
        'prefer-arrow-callback': 'error',
        'prefer-const': 'error',
        'prefer-rest-params': 'error',
        'radix': 'error',
      },
    },
    {
      files: ['*.ts'],
      excludedFiles: ['*.d.ts'],
      rules: {
        // Avoid some naming convention checks for JSX files where function components are capitalized
        '@typescript-eslint/naming-convention': [
          'error',
          { "selector": "class", "format": ["PascalCase"], leadingUnderscore: 'allow' },
          { "selector": "interface", "format": ["PascalCase"], leadingUnderscore: 'allow' },
          { "selector": "typeAlias", "format": ["PascalCase"], leadingUnderscore: 'allow' },
        ],
      },
    },
  ],
};
