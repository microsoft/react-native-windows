try {
  module.exports = require("@rnw-scripts/beachball-config");
} catch (ex) {
  if (ex.code === 'MODULE_NOT_FOUND') {
    console.error('Please run `yarn build` before running `yarn change`.');
    process.exit(1);
  }
}