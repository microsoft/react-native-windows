/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

'use strict';

const transformer = require('metro-react-native-babel-transformer');
module.exports = {
  process(file /*: string */) /*: {code: string, ...} */ {
    transformer.transform({
      filename: file,
      options: {
        inlineRequires: true,
      },
    });

    return transformer;
  },
};