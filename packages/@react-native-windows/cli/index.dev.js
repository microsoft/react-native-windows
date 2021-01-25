require('ts-node').register({transpileOnly: true, dir: __dirname});
module.exports = require('./src/index.ts');