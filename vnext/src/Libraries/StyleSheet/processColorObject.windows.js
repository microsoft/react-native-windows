/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {NativeOrDynamicColorType} from 'normalizeColorObject';

function processColorObject(
  color: NativeOrDynamicColorType,
): ?NativeOrDynamicColorType {
  return color;
}

module.exports = processColorObject;
