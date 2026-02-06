/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Dont use flow here, since this file is used by saveAssetPlugin.js which will run without flow transform
 * @format
 */

'use strict';

// Some windows machines may not have long paths enabled
// https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
// Assets in nested node_modules (common when using pnpm) - end up creating very long paths
// Using this function we shorten longer paths to prevent paths from hitting the path limit
function ensureShortPath(str) {
  if (str.length < 40) return str;

  const assetsPrefix = 'assets/';

  if (!str.startsWith(assetsPrefix)) {
    console.warn(`Unexpected asset uri - ${str} may not load correctly.`);
  }

  const postStr = str.slice(assetsPrefix.length);
  var hash = 0,
    i,
    chr;
  for (i = 0; i < postStr.length; i++) {
    chr = postStr.charCodeAt(i);
    hash = (hash << 5) - hash + chr;
    hash |= 0; // Convert to 32bit integer
  }
  return assetsPrefix + hash.toString();
}

export default ensureShortPath;
