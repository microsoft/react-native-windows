/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule AccessibilityInfo
 * @flow
 */
'use strict';
type ChangeEventName = $Enum<{
  change: string,
}>;

var warning = require('fbjs/lib/warning');

var AccessibilityInfo = {

  fetch: function(): Promise<bool> {
    return new Promise((resolve, reject) => {
      reject('AccessibilityInfo is not supported on this platform.');
    });
  },

  addEventListener: function (
    eventName: ChangeEventName,
    handler: Function
  ): void {
    warning(false, 'AccessibilityInfo is not supported on this platform.');
  },

  removeEventListener: function(
    eventName: ChangeEventName,
    handler: Function
  ): void {
    warning(false, 'AccessibilityInfo is not supported on this platform.');
  },

};

module.exports = AccessibilityInfo;
