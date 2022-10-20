module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: {tsconfigRootDir : __dirname},
  overrides: [
    {
      files: ['*.tsx'],
      // Also not clean
      rules: {
        '@typescript-eslint/no-unused-vars': 'off',
      },
    },
  ],
};
