/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Device info doesn't can't be a global set of data for win32
 * (probably not for android either).
 *
 * Since the API as designed by facebook can't work, we stub out all we can
 * @format
 * @flow
 */

'use strict';

import type {Spec} from './NativeDeviceInfo';

const DeviceInfo: Spec = {
  getConstants: () => ({Dimensions: {}}),
};

module.exports = DeviceInfo;
