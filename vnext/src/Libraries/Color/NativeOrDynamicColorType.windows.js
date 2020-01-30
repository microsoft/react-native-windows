/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

'use strict';

export type NativeOrDynamicColorType = {
  semantic?: string,
  dynamic?: {
    light: ?(string | number | NativeOrDynamicColorType),
    dark: ?(string | number | NativeOrDynamicColorType),
  },
};
