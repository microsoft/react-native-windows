/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';

export function resolveRnRoot(paths: string | string[] | undefined): string {
  return path.dirname(
    require.resolve('react-native/package.json', {
      paths: typeof paths === 'string' ? [paths] : paths,
    }),
  );
}

export function resolveRnCliRoot(paths: string | string[] | undefined): string {
  return path.dirname(
    require.resolve('@react-native-community/cli/package.json', {
      paths: typeof paths === 'string' ? [paths] : paths,
    }),
  );
}

export function resolveRnwRoot(paths: string | string[] | undefined): string {
  return path.dirname(
    require.resolve('react-native-windows/package.json', {
      paths: typeof paths === 'string' ? [paths] : paths,
    }),
  );
}

export function resolveRnwCliRoot(
  paths: string | string[] | undefined,
): string {
  return path.dirname(
    require.resolve('@react-native-windows/cli/package.json', {
      paths: typeof paths === 'string' ? [paths] : paths,
    }),
  );
}
