/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

function ensureSmallPath(str) {
  if (str.length < 40) return str;

  const assetsPrefix = 'assets/';

  if (!str.startsWith(assetsPrefix)) {
    console.warn(`Unexpected asset uri - ${str} may not load correctly.`);
  }

  str = str.slice(assetsPrefix.length);
  var hash = 0,
    i,
    chr;
  for (i = 0; i < str.length; i++) {
    chr = str.charCodeAt(i);
    hash = (hash << 5) - hash + chr;
    hash |= 0; // Convert to 32bit integer
  }
  return assetsPrefix + hash.toString();
}

module.exports = ensureSmallPath;
