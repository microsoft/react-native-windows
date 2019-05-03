using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;

namespace ReactNative.Tests
{
    class MockReactCallback : IReactCallback
    {
        public Action OnBatchCompleteHandler;
        public Action<int, int, JArray> InvokeHandler;
        public Func<int, int, JArray, JToken> InvokeSyncHandler;
        
        public void Invoke(int moduleId, int methodId, JArray parameters)
        {
            InvokeHandler?.Invoke(moduleId, methodId, parameters);
        }

        public JToken InvokeSync(int moduleId, int methodId, JArray parameters)
        {
            return InvokeSyncHandler?.Invoke(moduleId, methodId, parameters);
        }

        public void OnBatchComplete()
        {
            OnBatchCompleteHandler?.Invoke();
        }
    }
}
