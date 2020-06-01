/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @flow
 * @format
 */

'use strict';

import type {NativeOrDynamicColorType} from './normalizeColorObject';

function processColorObject(
  color: NativeOrDynamicColorType,
): ?NativeOrDynamicColorType {
  return color;
}

module.exports = processColorObject;
