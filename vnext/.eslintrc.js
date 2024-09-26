module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: {tsconfigRootDir : __dirname},
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
      files: ['*d.ts'],
      // Also not clean
      rules: {
        '@typescript-eslint/no-unused-vars': 'off',
      },
    },
  ],
};
