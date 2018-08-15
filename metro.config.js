"use strict";

var path = require("path");
const blacklist = require("metro-config/src/defaults/blacklist");
const defaultPolyfills = require("react-native/rn-get-polyfills");

var config = {
  resolver: {
    hasteImplModulePath: require.resolve('./jest/hasteImpl'),
  },
};

module.exports = config;
