/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

export type SkipTest = {skip: string};

export function skip(component: string) {
  return {skip: component};
}
