/**
 * Custom haste impl needed to deal with the yarn workspace
 *
 * This should be able to be removed once we move to RN 0.60, and haste goes away
 *
 * @format
 * @flow
 */

'use strict';
const path = require('path');
const fs = require('fs');

const rnPath = fs.realpathSync(
  path.resolve(require.resolve('react-native/package.json'), '..'),
);
const rnwPath = fs.realpathSync(
  path.resolve(require.resolve('react-native-windows/package.json'), '..'),
);
const ROOTS = [rnwPath + path.sep, rnPath + path.sep];

const BLACKLISTED_PATTERNS /*: Array<RegExp> */ = [
  /.*[\\\/]__(mocks|tests)__[\\\/].*/,
  /^Libraries[\\\/]Animated[\\\/]src[\\\/]polyfills[\\\/].*/,
  /^lib[\\/]Libraries[\\\/]Animated[\\\/]src[\\\/]polyfills[\\\/].*/,
  /^Libraries[\\\/]Renderer[\\\/]fb[\\\/].*/,
  /^lib[\\/]Libraries[\\\/]Renderer[\\\/]fb[\\\/].*/,
  /^wdio[\\/]*/,
  /^windows[\\/]*/,
];

const WHITELISTED_PREFIXES /*: Array<string> */ = [
  'IntegrationTests',
  'Libraries',
  'ReactAndroid',
  'RNTester',
];

const NAME_REDUCERS /*: Array<[RegExp, string]> */ = [
  // extract basename
  [/^(?:.*[\\\/])?([a-zA-Z0-9$_.-]+)$/, '$1'],
  // strip .js/.js.flow suffix
  [/^(.*)\.js(\.flow)?$/, '$1'],
  // strip platform suffix
  [/^(.*)\.(android|ios|native|windesktop|windows|macos|win32)$/, '$1'],
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
  if (!filePath.endsWith('.js') && !filePath.endsWith('.js.flow')) {
    return false;
  }

  const root = ROOTS.find(r => filePath.startsWith(r));
  if (!root) {
    return false;
  }

  filePath = filePath.substr(root.length);

  if (BLACKLISTED_PATTERNS.some(pattern => pattern.test(filePath))) {
    return false;
  }

  return WHITELISTED_PREFIXES.some(prefix => filePath.startsWith(prefix));
}

module.exports = haste;
