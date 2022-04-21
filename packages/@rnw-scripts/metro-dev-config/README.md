# @rnw-scripts/metro-dev-config

This package allows generating metro configs layered on top of [rnx-kit](https://github.com/microsoft/rnx-kit), to be
used by RNW development packages. This allows reduced duplication, better
diagnostics, and generic package symlinking support.

## Usage

```js
// metro.config.js
const { makeMetroConfig } = require("@rnw-scripts/metro-dev-config");

module.exports = makeMetroConfig({
  projectRoot: __dirname,
});
```