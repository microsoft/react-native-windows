/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as parser from '@babel/parser';
import * as fs from 'fs';
import * as path from 'path';
import * as walkdir from 'walkdir';

import traverse from '@babel/traverse';
import {File} from '@babel/types';

import {block, skip, TestComponent, TestBlock} from './TestDescription';

const AUTO_REGISTER_FUNCS = ['componentTest', 'functionTest'];

/**
 * Searches for test components defined using special function designated for
 * automatic registration. Tests are discovered if they are called using a
 * string literal for their test name
 *
 * @param rootPath path to a directory containing files to search
 * @returns the names of components to register
 */
export default (rootPath: string): TestBlock[] => {
  const typeScriptFiles = enumerateTypeScriptFiles(rootPath);

  return typeScriptFiles.flatMap(file => {
    const fileContents = fs.readFileSync(file);

    const ast = parser.parse(fileContents.toString(), {
      plugins: ['typescript', 'jsx'],
      sourceType: 'module',
    });

    const tests = extractComponents(ast);
    if (tests.length > 0) {
      const filename = path.parse(file).name;
      return [block(filename, tests)];
    } else {
      return [];
    }
  });
};

/**
 * Finds all TypeScript files within a directory
 * @param rootPath path to a directory containing files to search
 */
function enumerateTypeScriptFiles(rootPath: string): string[] {
  const files = walkdir.sync(rootPath, {return_object: true});
  return Object.entries(files).flatMap(([filePath, stat]) => {
    if (
      (stat.isFile() && filePath.endsWith('.ts')) ||
      filePath.endsWith('.tsx')
    ) {
      return [filePath];
    } else {
      return [];
    }
  });
}

/**
 * Finds declared components within an AST
 * @param file a babel AST
 */
function extractComponents(file: File): TestComponent[] {
  const components: TestComponent[] = [];

  traverse(file, {
    // eslint-disable-next-line @typescript-eslint/naming-convention
    CallExpression: nodePath => {
      if (
        nodePath.node.callee.type === 'Identifier' &&
        AUTO_REGISTER_FUNCS.includes(nodePath.node.callee.name) &&
        nodePath.node.arguments.length > 0 &&
        nodePath.node.arguments[0].type === 'StringLiteral'
      ) {
        components.push(nodePath.node.arguments[0].value);
      } else if (
        nodePath.node.callee.type === 'MemberExpression' &&
        nodePath.node.callee.object.type === 'Identifier' &&
        AUTO_REGISTER_FUNCS.includes(nodePath.node.callee.object.name) &&
        nodePath.node.callee.property.type === 'Identifier' &&
        nodePath.node.callee.property.name === 'skip' &&
        nodePath.node.arguments.length > 0 &&
        nodePath.node.arguments[0].type === 'StringLiteral'
      ) {
        components.push(skip(nodePath.node.arguments[0].value));
      }
    },
  });

  return components;
}
