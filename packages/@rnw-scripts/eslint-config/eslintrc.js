/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

module.exports = { 
  extends: [
    "@react-native-community",
    "prettier"
  ],
  rules: { 
    "react-native/no-inline-styles" : "off",
  },
  ignorePatterns: [
    'lib/**',
    'lib-commonjs/**'
  ]
};
