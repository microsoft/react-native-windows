/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

var Settings = {
  get(key: string): mixed {
    console.warn('Settings is not yet supported on Win32');
    return null;
  },

  set(settings: Object) {
    console.warn('Settings is not yet supported on Win32');
  },

  watchKeys(keys: string | Array<string>, callback: Function): number {
    console.warn('Settings is not yet supported on Win32');
    return -1;
  },

  clearWatch(watchId: number) {
    console.warn('Settings is not yet supported on Win32');
  },
};

module.exports = Settings;
