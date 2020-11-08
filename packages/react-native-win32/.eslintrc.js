module.exports = {
  extends: ['@rnw-scripts'],
  rules: {
    // Not clean
    "react-hooks/exhaustive-deps": "warn",
    "react-hooks/rules-of-hooks": "warn",

    // @typescript/eslint rules requiring compilation don't work with noEmit
    '@typescript-eslint/no-floating-promises': 'off',
    '@typescript-eslint/no-misused-promises': 'off',
  },
  parserOptions: {project : null},
};