/**
 * Copyright (c) 2015-present, Microsoft Corporation.
 * All rights reserved.
 *
 * @providesModule BackgroundRegistrationModule
 * @flow
 */

var NativeModules = require('NativeModules');
var BatchedBridge = require('BatchedBridge');

const BackgroundRegistrationModule = {
  unregister(id) {
    NativeModules.Background.unregister(id);
  }
}

BatchedBridge.registerCallableModule('BackgroundRegistrationModule', BackgroundRegistrationModule);

module.exports = BackgroundRegistrationModule;
