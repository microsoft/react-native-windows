/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

'use strict';

const generate = require('@babel/generator').default;

// Require @react-native/metro-babel-transformer from @react-native/metro-config to ensure we pickup the version that @react-native/metro-config uses
const transformer = require(require.resolve('@react-native/metro-babel-transformer', {paths: [require('path').dirname(require.resolve('@react-native/metro-config/package.json'))]}));
module.exports = {
  process(src /*: string */, file /*: string */) /*: {code: string, ...} */ {
    const {ast} = transformer.transform({
      filename: file,
      options: {
        inlineRequires: true,
      },
      src,
    });

    return generate(ast, {}, src);
  },
};
