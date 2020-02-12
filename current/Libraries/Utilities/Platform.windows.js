/**
 * Copyright (c) Microsoft Corporation.
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
