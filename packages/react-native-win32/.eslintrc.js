module.exports = {
    extends: ['@rnw-scripts'],
    rules: {
      // Not clean
      "react-hooks/exhaustive-deps": "warn",
      "react-hooks/rules-of-hooks": "warn",
    },
    parserOptions: {tsconfigRootDir : __dirname},
};