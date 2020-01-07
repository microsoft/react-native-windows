/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * Device info doesn't can't be a global set of data for win32
 * (probably not for android either).
 *
 * Since the API as designed by facebook can't work, we just return null here to crash when used
 * @format
 * @flow
 */

'use strict';

module.exports = {isIPhoneX_deprecated: false};
