/**
 * Copyright (c) 2015-present, Microsoft Corporation.
 * All rights reserved.
 *
 * @providesModule AppBackgroundModule
 * @flow
 */

var NativeModules = require('NativeModules');
var BatchedBridge = require('BatchedBridge');

const AppBackgroundModule = {
  doWork(taskId, deferralId) {
    console.log("In JS doing background work...");
    let count = 0;
    var handle = setInterval(() => {
      console.log(`In timer doing background work: ${count}`);
      if (++count == 3) {
        NativeModules.Background.completeDeferral(deferralId);
        clearInterval(handle);
      }
    }, 1000);
  }
}

BatchedBridge.registerCallableModule('AppBackgroundModule', AppBackgroundModule);

module.exports = AppBackgroundModule;
