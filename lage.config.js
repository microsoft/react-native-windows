/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

module.exports = {
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