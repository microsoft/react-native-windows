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

<<<<<<< Upstream
export type {DeviceInfoConstants} from './NativeDeviceInfo';

import NativeDeviceInfo from './NativeDeviceInfo';
=======
import type {Spec} from './NativeDeviceInfo';
>>>>>>> Override

const DeviceInfo: Spec = {
  getConstants: () => ({Dimensions: {}}),
};

export default DeviceInfo;
