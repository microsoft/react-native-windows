module.exports = {
    extends: ['@rnw-scripts'],

    // TS 3.8 imports don't work with the old version of prettier the repo is
    // defaulting to. Ignore the CLI for now.
    ignorePatterns: ['src/Cli.ts']
};
