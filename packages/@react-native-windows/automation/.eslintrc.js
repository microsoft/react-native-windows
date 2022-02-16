module.exports = {
  extends: ['@rnw-scripts'],
  parserOptions: {tsconfigRootDir : __dirname},
  
  // Workaround "export type" being unsupported with prettier 1.x
  ignorePatterns: ['index.ts'],
};
