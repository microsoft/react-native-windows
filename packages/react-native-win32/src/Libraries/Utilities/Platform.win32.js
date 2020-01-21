/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

const NativeModules = require('NativeModules');

const Platform = {
  OS: 'win32',
  get Version() {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.Version;
  },
  get isTesting(): boolean {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.isTesting;
  },
  select: (obj: Object) => ('win32' in obj ? obj.win32 : obj.default),
  get isTV() {
    const constants = NativeModules.PlatformConstants;
    return constants ? constants.interfaceIdiom === 'tv' : false;
  },
};

module.exports = Platform;
