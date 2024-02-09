/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

'use strict';

const {transformFromAstSync} = require('@babel/core');

const generate = require('@babel/generator').default;
const transformer = require('@react-native/metro-babel-transformer');
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
