/**
 * Copyright (c) 2015-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 * 
 * Portions copyright for react-native-windows:
 * 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * 
 * @providesModule Settings
 * @flow
 */
'use strict';

var Settings = {
  get(key: string): mixed {
    console.warn('Settings is not yet supported on Windows');
    return null;
  },

  set(settings: Object) {
    console.warn('Settings is not yet supported on Windows');
  },

  watchKeys(keys: string | Array<string>, callback: Function): number {
    console.warn('Settings is not yet supported on Windows');
    return -1;
  },

  clearWatch(watchId: number) {
    console.warn('Settings is not yet supported on Windows');
  },
};

module.exports = Settings;
