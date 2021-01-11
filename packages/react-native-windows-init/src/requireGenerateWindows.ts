/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {GenerateWindows, GenerateWindows62} from './GenerateWindowsType';

/**
 * Returns the Windows template generator function, normalizing the function
 * to any differences across RN versions.
 */
export default function requireGenerateWindows(): GenerateWindows {
  try {
    // Try the path for 0.63+
    const requirePath = require.resolve('react-native-windows/generate', {
      paths: [process.cwd()],
    });

    return require(requirePath);
  } catch {
    // Fall back to trying the older path
    const requirePath = require.resolve(
      'react-native-windows/local-cli/generate-windows',
      {paths: [process.cwd()]},
    );

    const generate62: GenerateWindows62 = require(requirePath);
    return async (...args) => generate62(...args);
  }
}
