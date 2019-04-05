// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

function require(name) {
  return this[name];
}

var FunctionCalls = new Array();
var CallbackCalls = new Array();
var BatchedBridge =
  {
    'callFunctionReturnFlushedQueue': function (moduleId, methodId, args) {
      FunctionCalls.push([moduleId, methodId, args]);
      if (moduleId === 'SyncModule' && methodId === 'test') {
        nativeCallSyncHook && nativeCallSyncHook(...args);
      }

      return [[], [], []];
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
