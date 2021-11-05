/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

 const os = require("os");

module.exports = {
  concurrency: os.cpus().length * 2,
  pipeline: {
    build: ["^build"],
    lint: ["build"],
    'lint:fix': [],
    test: ["build"],
    format: [],
    'format:verify': [],
    clean: [],
  },
  cacheOptions: {
    outputGlob: [
      'js/**',
      "lib/**",
      "lib-commonjs/**",
    ],
  }
};