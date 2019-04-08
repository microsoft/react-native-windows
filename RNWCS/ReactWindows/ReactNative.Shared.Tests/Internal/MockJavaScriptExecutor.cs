// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;

namespace ReactNative.Tests
{
    class MockJavaScriptExecutor : IJavaScriptExecutor
    {
        public Func<string, string, JArray, JToken> OnCallFunctionReturnFlushedQueue { get; set; } = EmptyCallFunctionReturnFlushedQueue;
        public Func<int, JArray, JToken> OnInvokeCallbackAndReturnFlushedQueue { get; set; } = EmptyInvokeCallbackAndReturnFlushedQueue;
        public Func<JToken> OnFlushQueue { get; set; } = EmptyFlushQueue;

        public Action<string, string> OnRunScript { get; set; } = EmptyRunScript;
        public Action<string, string> OnSetGlobalVariable { get; set; } = EmptySetGlobalVariable;
        public Action OnDispose { get; set; } = EmptyAction;

        private static readonly Action EmptyAction = () => { };
        private static readonly Action<string, string> EmptyRunScript = (_, __) => { };
        private static readonly Action<string, string> EmptySetGlobalVariable = (_, __) => { };
        private static readonly Func<string, string, JArray, JToken> EmptyCallFunctionReturnFlushedQueue = (_, __, ___) => { throw new NotImplementedException(); };
        private static readonly Func<int, JArray, JToken> EmptyInvokeCallbackAndReturnFlushedQueue = (_, __) => { throw new NotImplementedException(); };
        private static readonly Func<JToken> EmptyFlushQueue = () => { throw new NotImplementedException(); };

        public void Initialize() { }

        public void RunScript(string sourcePath, string sourceUrl)
        {
            OnRunScript(sourcePath, sourceUrl);
        }

        public void SetFlushQueueImmediate(Action<JToken> flushQueueImmediate)
        {
        }

        public void SetGlobalVariable(string propertyName, string value)
        {
            OnSetGlobalVariable(propertyName, value);
        }

        public void SetCallSyncHook(Func<int, int, JArray, JToken> callSyncHook)
        {
        }

        public void Dispose()
        {
            OnDispose();
        }

        public JToken CallFunctionReturnFlushedQueue(string moduleName, string methodName, JArray arguments)
        {
            return OnCallFunctionReturnFlushedQueue(moduleName, methodName, arguments);
        }

        public JToken InvokeCallbackAndReturnFlushedQueue(int callbackId, JArray arguments)
        {
            return OnInvokeCallbackAndReturnFlushedQueue(callbackId, arguments);
        }

        public JToken FlushedQueue()
        {
            return OnFlushQueue();
        }
    }
}
