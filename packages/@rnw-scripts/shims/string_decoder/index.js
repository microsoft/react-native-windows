// Re-export Node's built-in string_decoder. readable-stream (pulled by the
// archiver test-tooling chain) depends on the npm "string_decoder" mirror, which
// is unmaintained and flagged by Component Governance. A Yarn resolution points
// that package at this shim so the built-in is used instead; the API is identical.
module.exports = require('node:string_decoder');
