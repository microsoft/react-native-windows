module.exports = {
  extends: ['@rnw-scripts'],
  rules: {
    // @typescript/eslint rules requiring compilation don't work with noEmit
    '@typescript-eslint/no-floating-promises': 'off',
    '@typescript-eslint/no-misused-promises': 'off',
  },
  parserOptions: {project : null},
};
