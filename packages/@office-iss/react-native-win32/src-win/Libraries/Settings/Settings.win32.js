/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
<<<<<<< Upstream

import Platform from '../Utilities/Platform';

let Settings: {
  get(key: string): any,
  set(settings: Object): void,
  watchKeys(keys: string | Array<string>, callback: () => void): number,
  clearWatch(watchId: number): void,
  ...
=======
'use strict';

var Settings = {
  get(key: string): any {
    console.warn('Settings is not yet supported on Win32');
    return null;
  },

  set(settings: Object) {
    console.warn('Settings is not yet supported on Win32');
  },

  watchKeys(keys: string | Array<string>, callback: () => void): number {
    console.warn('Settings is not yet supported on Win32');
    return -1;
  },

  clearWatch(watchId: number) {
    console.warn('Settings is not yet supported on Win32');
  },
>>>>>>> Override
};

if (Platform.OS === 'ios') {
  Settings = require('./Settings').default;
} else {
  Settings = require('./SettingsFallback').default;
}

export default Settings;
