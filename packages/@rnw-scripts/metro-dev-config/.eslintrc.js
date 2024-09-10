module.exports = {
    extends: ['@rnw-scripts'],
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
  ],
};
