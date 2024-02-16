/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

'use strict';

const path = require('path');

const {transformFromAstSync} = require('@babel/core');

const generate = require('@babel/generator').default;

// Require @react-native/metro-babel-transformer from @react-native/metro-config to ensure we pickup the version that @react-native/metro-config uses
const transformer = require(require.resolve(
  '@react-native/metro-babel-transformer',
  {paths: [path.dirname(require.resolve('@react-native/metro-config'))]},
));
module.exports = {
  process(src /*: string */, file /*: string */) /*: {code: string, ...} */ {
    let {ast} = transformer.transform({
      filename: file,
      options: {
        inlineRequires: true,
      },
      src,
    });
    const babelTransformResult = transformFromAstSync(ast, src, {
      ast: true,
      retainLines: true,
      plugins: [require('metro-transform-plugins').inlineRequiresPlugin],
      sourceType: 'module',
    });
    ast = babelTransformResult.ast;
    return generate(ast, {}, src);
  },
};
