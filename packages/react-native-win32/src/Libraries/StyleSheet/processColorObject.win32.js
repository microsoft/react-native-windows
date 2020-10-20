/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * @format
 * @flow
 */
'use strict';

const processColor = require('./processColor');

import type {NativeOrDynamicColorType} from './NativeOrDynamicColorType';
import {handleColorObject} from './NativeOrDynamicColorType';

function processColorObject(
  color?: NativeOrDynamicColorType,
): ?NativeOrDynamicColorType {
  return handleColorObject(color, processColor);
}

module.exports = processColorObject;
// ]TODO(macOS ISS#2323203)
