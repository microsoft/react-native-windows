module.exports = {
    extends: ['@rnw-scripts'],
    rules: {
      // Not clean
      "react-hooks/exhaustive-deps": "warn",
      "react-hooks/rules-of-hooks": "warn",
    },
    overrides: [
      {
        files: ['*.js', '*.js.flow', '*.jsx'],
        parser: 'hermes-eslint',
        rules: {
          // These rules are not required with hermes-eslint
          'ft-flow/define-flow-type': 0,
          'ft-flow/use-flow-type': 0,
          // flow handles this check for us, so it's not required
          'no-undef': 0,
        },
      },
      {
        files: ['*.ts', '*.tsx'],
        excludedFiles: ['*.d.ts'],
        // Also not clean
        rules: {
          '@typescript-eslint/no-unnecessary-condition': 'off',
          'no-restricted-syntax': 'off',
          '@typescript-eslint/no-unused-vars': 'off',
          '@typescript-eslint/prefer-readonly': 'off',
        },
      },
    ],
    parserOptions: {tsconfigRootDir : __dirname},
};