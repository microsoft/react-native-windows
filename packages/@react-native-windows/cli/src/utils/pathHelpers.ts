/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';

export function resolveRnwRoot(paths: string | string[] | undefined): string {
  return path.dirname(
    require.resolve('react-native-windows/package.json', {
      paths: typeof paths === 'string' ? [paths] : paths,
    }),
  );
}
