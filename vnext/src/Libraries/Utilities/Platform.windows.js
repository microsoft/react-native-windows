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
  OS: 'windows',
  get Version() {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.Version;
  },
  get isTesting(): boolean {
    const constants = NativeModules.PlatformConstants;
    return constants && constants.isTesting;
  },
  select: (obj: Object) => ('windows' in obj ? obj.windows : obj.default),
};

module.exports = Platform;
