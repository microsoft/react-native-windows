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
        public Action<string, JToken> OnSetGlobalVariable { get; set; } = EmptySetGlobalVariable;
        public Action OnDispose { get; set; } = EmptyAction;

        private static readonly Action EmptyAction = () => { };
        private static readonly Action<string, string> EmptyRunScript = (_, __) => { };
        private static readonly Action<string, JToken> EmptySetGlobalVariable = (_, __) => { };
        private static readonly Func<string, string, JArray, JToken> EmptyCallFunctionReturnFlushedQueue = (_, __, ___) => { throw new NotImplementedException(); };
        private static readonly Func<int, JArray, JToken> EmptyInvokeCallbackAndReturnFlushedQueue = (_, __) => { throw new NotImplementedException(); };
        private static readonly Func<JToken> EmptyFlushQueue = () => { throw new NotImplementedException(); };

        public void Initialize() { }

        public void RunScript(string script, string sourceUrl)
        {
            OnRunScript(script, sourceUrl);
        }

        public void SetGlobalVariable(string propertyName, JToken value)
        {
            OnSetGlobalVariable(propertyName, value);
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
