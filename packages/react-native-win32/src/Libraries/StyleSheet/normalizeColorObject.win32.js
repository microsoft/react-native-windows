/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * @format
 * @flow
 */
'use strict';

const normalizeColor = require('./normalizeColor');

import type {NativeOrDynamicColorType} from './NativeOrDynamicColorType';
import {handleColorObject} from './NativeOrDynamicColorType';

function normalizeColorObject(
  color?: NativeOrDynamicColorType,
): ?NativeOrDynamicColorType {
  return handleColorObject(color, normalizeColor);
}

module.exports = normalizeColorObject;
