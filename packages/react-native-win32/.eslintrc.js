module.exports = {
    extends: ['@rnw-scripts'],
    rules: {
      // Not clean
      "react-hooks/exhaustive-deps": "warn",
      "react-hooks/rules-of-hooks": "warn",
    },
    overrides: [
      {
        files: ['*.ts', '*.tsx'],
        excludedFiles: ['*.d.ts'],
        // Also not clean
        rules: {
          '@typescript-eslint/no-unnecessary-condition': 'off',
          'no-restricted-syntax': 'off',
        },
      },
    ],
    parserOptions: {tsconfigRootDir : __dirname},
};