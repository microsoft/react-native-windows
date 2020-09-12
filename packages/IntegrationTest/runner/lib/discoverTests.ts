/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as babelParser from '@babel/parser';
import * as fs from 'fs';
import * as walkdir from 'walkdir';

import {Program} from '@babel/types';

import {skip, SkipTest} from './SkipTest';

const AUTO_REGISTER_FUNCS = ['componentTest', 'functionTest'];

/**
 * Searches for test components defined using special function designated for
 * automatic registration. Tests are discovered if:
 *
 * 1. They are called using a string literal for their compoent name
 * 2. They're defined at top-level scope for the module
 *
 * @param rootPath path to a directory containing files to search
 * @returns the names of components to register
 */
export default (rootPath: string): Array<string | SkipTest> => {
  const typeScriptFiles = enumerateTypeScriptFiles(rootPath);

  return typeScriptFiles.flatMap(file => {
    const fileContents = fs.readFileSync(file);

    const ast = babelParser.parse(fileContents.toString(), {
      plugins: ['typescript', 'jsx'],
      sourceType: 'module',
    });

    return extractComponents(ast.program);
  });
};

/**
 * Finds all TypeScript files within a directory
 * @param rootPath path to a directory containing files to search
 */
function enumerateTypeScriptFiles(rootPath: string): string[] {
  const files = walkdir.sync(rootPath, {return_object: true});
  return Object.entries(files).flatMap(([path, stat]) => {
    if ((stat.isFile && path.endsWith('.ts')) || path.endsWith('.tsx')) {
      return [path];
    } else {
      return [];
    }
  });
}

/**
 * Finds declared components within an AST
 * @param program a babel AST program
 */
function extractComponents(program: Program): Array<string | SkipTest> {
  return program.body.flatMap(
    (node): Array<string | SkipTest> => {
      if (
        node.type === 'ExpressionStatement' &&
        node.expression.type === 'CallExpression' &&
        node.expression.callee.type === 'Identifier' &&
        AUTO_REGISTER_FUNCS.includes(node.expression.callee.name) &&
        node.expression.arguments.length > 0 &&
        node.expression.arguments[0].type === 'StringLiteral'
      ) {
        return [node.expression.arguments[0].value];
      } else if (
        node.type === 'ExpressionStatement' &&
        node.expression.type === 'CallExpression' &&
        node.expression.callee.type === 'MemberExpression' &&
        node.expression.callee.object.type === 'Identifier' &&
        AUTO_REGISTER_FUNCS.includes(node.expression.callee.object.name) &&
        node.expression.callee.property.type === 'Identifier' &&
        node.expression.callee.property.name === 'skip' &&
        node.expression.arguments.length > 0 &&
        node.expression.arguments[0].type === 'StringLiteral'
      ) {
        return [skip(node.expression.arguments[0].value)];
      } else {
        return [];
      }
    },
  );
}
