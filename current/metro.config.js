"use strict";

var path = require("path");

var config = {
  resolver: {
    platforms: [ 'windows' ],
    hasteImplModulePath: path.resolve(__dirname, 'jest/hasteImpl'),
  },
};

module.exports = config;
