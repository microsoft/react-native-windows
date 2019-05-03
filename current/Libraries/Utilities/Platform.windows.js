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
 * @providesModule Platform
 * @flow
 */

'use strict';

const Platform = {
  OS: 'windows',
  select: (obj: Object) => 'windows' in obj ? obj.windows : obj.default,
};

module.exports = Platform;
