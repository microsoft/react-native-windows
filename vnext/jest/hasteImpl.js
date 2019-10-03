/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

const path = require('path');

function createHaste(roots) {
  const ROOTS = roots;

  const IGNORE_PATTERNS /*: Array<RegExp> */ = [
    /.*[\\\/]__(mocks|tests)__[\\\/].*/,
    /^Libraries[\\\/]Animated[\\\/]src[\\\/]polyfills[\\\/].*/,
    /^Libraries[\\\/]Renderer[\\\/]fb[\\\/].*/,
  ];

  const INCLUDE_PREFIXES /*: Array<string> */ = [
    'IntegrationTests',
    'Libraries',
    'lib' + path.sep + 'Libraries',
    'ReactAndroid',
    'RNTester',
  ];

  const NAME_REDUCERS /*: Array<[RegExp, string]> */ = [
    // extract basename
    [/^(?:.*[\\\/])?([a-zA-Z0-9$_.-]+)$/, '$1'],
    // strip .js/.js.flow suffix
    [/^(.*)\.js(\.flow)?$/, '$1'],
    // strip platform suffix
    [/^(.*)\.(android|ios|native|windesktop|windows|macos)$/, '$1'],
  ];

  const haste = {
    /*
     * @return {string|void} hasteName for module at filePath; or undefined if
     *                       filePath is not a haste module
     */
    getHasteName(
      filePath /*: string */,
      sourceCode /*: ?string */,
    ) /*: string | void */ {
      if (!isHastePath(filePath)) {
        return undefined;
      }
      const hasteName = NAME_REDUCERS.reduce(
        (name, [pattern, replacement]) => name.replace(pattern, replacement),
        filePath,
      );

      return hasteName;
    },
  };

  function isHastePath(filePath /*: string */) /*: boolean */ {
    const root = ROOTS.find(r => filePath.startsWith(r));
    if (!root) {
      return false;
    }

    filePath = filePath.substr(root.length);
    if (IGNORE_PATTERNS.some(pattern => pattern.test(filePath))) {
      return false;
    }

    return INCLUDE_PREFIXES.some(prefix => filePath.startsWith(prefix));
  }

  return haste;
}

module.exports = createHaste([path.resolve(__dirname, '..') + path.sep]);
