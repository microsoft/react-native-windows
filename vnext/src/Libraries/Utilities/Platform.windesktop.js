/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const NativeModules = require('NativeModules');

const Platform = {
  OS: 'windesktop',
  get Version() {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.Version;
  },
  get isTesting(): boolean {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.isTesting;
  },
  select: (obj: Object) => ('windesktop' in obj ? obj.windesktop : obj.default),
};

module.exports = Platform;
