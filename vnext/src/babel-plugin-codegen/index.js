/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

'use strict';

let flowParser, typeScriptParser, RNCodegen;

try {
  flowParser = require('react-native-codegen/src/parsers/flow');
  typeScriptParser = require('react-native-codegen/src/parsers/typescript');
  RNCodegen = require('react-native-codegen/src/generators/RNCodegen');
} catch (e) {
  // Fallback to lib when source doesn't exit (e.g. when installed as a dev dependency)
  flowParser = require('react-native-codegen/lib/parsers/flow');
  typeScriptParser = require('react-native-codegen/lib/parsers/typescript');
  RNCodegen = require('react-native-codegen/lib/generators/RNCodegen');
}

// [Win - changes to use local react-native-codegen from tscodegen, which has the flow types removed
flowParser = require('../../../node_modules/react-native-tscodegen/lib/rncodegen/src/parsers/flow');
RNCodegen = {
  generateViewConfig: ({libraryName, schema}) => {
    // schemaValidator.validate(schema);
    const result = require('./GenerateViewConfigJs')
      .generate(libraryName, schema)
      .values()
      .next();

    if (typeof result.value !== 'string') {
      throw new Error(`Failed to generate view config for ${libraryName}`);
    }

    return result.value;
  },
};
// Win]

const {basename} = require('path');

function parse(filename, code) {
  if (filename.endsWith('js')) {
    return flowParser.parseString(code);
  }

  if (filename.endsWith('ts')) {
    return typeScriptParser.parseString(code);
  }

  throw new Error(
    `Unable to parse file '${filename}'. Unsupported filename extension.`,
  );
}

function generateViewConfig(filename, code) {
  const schema = parse(filename, code);

  const libraryName = basename(filename).replace(
    /NativeComponent\.(js|ts)$/,
    '',
  );
  return RNCodegen.generateViewConfig({
    schema,
    libraryName,
  });
}

function isCodegenDeclaration(declaration) {
  if (!declaration) {
    return false;
  }

  if (
    declaration.left &&
    declaration.left.left &&
    declaration.left.left.name === 'codegenNativeComponent'
  ) {
    return true;
  } else if (
    declaration.callee &&
    declaration.callee.name &&
    declaration.callee.name === 'codegenNativeComponent'
  ) {
    return true;
  } else if (
    declaration.type === 'TypeCastExpression' &&
    declaration.expression &&
    declaration.expression.callee &&
    declaration.expression.callee.name &&
    declaration.expression.callee.name === 'codegenNativeComponent'
  ) {
    return true;
  }

  return false;
}

module.exports = function ({parse, types: t}) {
  return {
    pre(state) {
      this.code = state.code;
      this.filename = state.opts.filename;
      this.defaultExport = null;
      this.commandsExport = null;
      this.codeInserted = false;
    },
    visitor: {
      ExportNamedDeclaration(path) {
        if (this.codeInserted) {
          return;
        }

        if (
          path.node.declaration &&
          path.node.declaration.declarations &&
          path.node.declaration.declarations[0]
        ) {
          const firstDeclaration = path.node.declaration.declarations[0];

          if (firstDeclaration.type === 'VariableDeclarator') {
            if (
              firstDeclaration.init &&
              firstDeclaration.init.type === 'CallExpression' &&
              firstDeclaration.init.callee.type === 'Identifier' &&
              firstDeclaration.init.callee.name === 'codegenNativeCommands'
            ) {
              if (
                firstDeclaration.id.type === 'Identifier' &&
                firstDeclaration.id.name !== 'Commands'
              ) {
                throw path.buildCodeFrameError(
                  "Native commands must be exported with the name 'Commands'",
                );
              }
              this.commandsExport = path;
              return;
            } else {
              if (firstDeclaration.id.name === 'Commands') {
                throw path.buildCodeFrameError(
                  "'Commands' is a reserved export and may only be used to export the result of codegenNativeCommands.",
                );
              }
            }
          }
        } else if (path.node.specifiers && path.node.specifiers.length > 0) {
          path.node.specifiers.forEach(specifier => {
            if (
              specifier.type === 'ExportSpecifier' &&
              specifier.local.type === 'Identifier' &&
              specifier.local.name === 'Commands'
            ) {
              throw path.buildCodeFrameError(
                "'Commands' is a reserved export and may only be used to export the result of codegenNativeCommands.",
              );
            }
          });
        }
      },
      ExportDefaultDeclaration(path, state) {
        if (isCodegenDeclaration(path.node.declaration)) {
          this.defaultExport = path;
        }
      },

      Program: {
        exit(path) {
          if (this.defaultExport) {
            const viewConfig = generateViewConfig(this.filename, this.code);
            this.defaultExport.replaceWithMultiple(
              // [Win adding filename param see: https://github.com/facebook/react-native/pull/29230
              parse(viewConfig, {
                babelrc: false,
                browserslistConfigFile: false,
                configFile: false,
                filename: this.filename,
              }).program.body, // Win]
            );
            if (this.commandsExport != null) {
              this.commandsExport.remove();
            }
            this.codeInserted = true;
          }
        },
      },
    },
  };
};
