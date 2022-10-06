module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: {tsconfigRootDir : __dirname},
  ignorePatterns: ['/js'],
  overrides: [
    {
      files: ['*.tsx'],
      // Also not clean
      rules: {
        '@typescript-eslint/no-unused-vars': 'off',
        '@typescript-eslint/prefer-readonly': 'off',
      },
    },
  ],
};
