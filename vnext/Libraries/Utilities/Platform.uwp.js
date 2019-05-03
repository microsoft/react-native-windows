// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


'use strict';

const NativeModules = require('NativeModules');

const Platform = {
  OS: 'uwp',
  get Version() {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.Version;
  },
  get isTesting(): boolean {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.isTesting;
  },
  select: (obj: Object) => 'uwp' in obj ? obj.uwp : obj.default,
};

module.exports = Platform;
