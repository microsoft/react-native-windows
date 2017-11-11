/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule Cursor
 * @flow
 */
'use strict';

const NativeModules = require('NativeModules');

var Cursor = {
  setCursor: function(
    cursor: ?string
  ): void {
    NativeModules.Cursor.setCursor(cursor)
  }
}

module.exports = Cursor;
