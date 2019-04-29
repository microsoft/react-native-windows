// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

'use strict';

var Settings = {
  get(key: string): mixed {
    console.warn('Settings is not yet supported on windesktop');
    return null;
  },

  set(settings: Object) {
    console.warn('Settings is not yet supported on windesktop');
  },

  watchKeys(keys: string | Array<string>, callback: Function): number {
    console.warn('Settings is not yet supported on windesktop');
    return -1;
  },

  clearWatch(watchId: number) {
    console.warn('Settings is not yet supported on windesktop');
  },
};

module.exports = Settings;
