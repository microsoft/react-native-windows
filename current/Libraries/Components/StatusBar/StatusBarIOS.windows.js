/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @providesModule StatusBarIOS
 * @flow
 */
'use strict';

const NativeEventEmitter = require('NativeEventEmitter');

module.exports = new NativeEventEmitter('StatusBarManager');
