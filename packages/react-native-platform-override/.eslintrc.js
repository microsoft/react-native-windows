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

    // @react-native-community doesn't seem to include the
    // eslint-config-prettier blacklist which can sometimes lead to conflicting
    // rules. Make sure to include the exclusions here.
    "prettier",
  ],
  rules:{ 
    "sort-imports": "warn",
}};
