// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

function require(name) {
  return this[name];
}

// Extract the remoteRootView method from config
var modconfig = __fbBatchedBridgeConfig.remoteModuleConfig;
var uiManagerModuleId = modconfig.findIndex(descr => descr[0] === 'UIManager');
var uiManagerConfig = modconfig[uiManagerModuleId];
var removeRootMethodId = uiManagerConfig[2].findIndex(name => name === 'removeRootView');

var FunctionCalls = new Array();
var CallbackCalls = new Array();
var BatchedBridge =
  {
    'callFunctionReturnFlushedQueue': function (moduleId, methodId, args) {
      FunctionCalls.push([moduleId, methodId, args]);

      if (moduleId === 'AppRegistry' &&
        methodId === 'unmountApplicationComponentAtRootTag') {
        return [[uiManagerModuleId], [removeRootMethodId], [[args[0]]]];
      } else {
        return [[], [], []];
      }
    },
    'invokeCallbackAndReturnFlushedQueue': function (callbackId, args) {
      CallbackCalls.push([callbackId, args]);
      return [[], [], []];
    },
    'flushedQueue': function (args) {
      return [[], [], []];
    }
  };

var __fbBatchedBridge = BatchedBridge;


