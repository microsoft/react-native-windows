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
      if (moduleId === 'FlushQueueImmediateModule' && methodId === 'test') {
        for (var i = 0; i < args[0]; ++i) {
          nativeFlushQueueImmediate && nativeFlushQueueImmediate(args[1]);
        }
      }

      return [[], [], []];
    },
    'invokeCallbackAndReturnFlushedQueue': function (callbackId, args) {
      CallbackCalls.push([callbackId, args]);
      return [[], [], []];
    },
    'flushedQueue': function (args) {
      return null;
    }
  };

var __fbBatchedBridge = BatchedBridge;
