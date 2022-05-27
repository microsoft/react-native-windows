/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

'use strict';

const generate = require('@babel/generator').default;
const transformer = require('metro-react-native-babel-transformer');
module.exports = {
  process(file /*: string */) /*: {code: string, ...} */ {
    const {ast} = transformer.transform({
      filename: file,
      options: {
        inlineRequires: true,
      },
    });

    return generate(
      ast,
    );
  },
};