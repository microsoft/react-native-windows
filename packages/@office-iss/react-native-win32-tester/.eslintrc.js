module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: {tsconfigRootDir : __dirname},
  ignorePatterns: ['/js'],
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
};
