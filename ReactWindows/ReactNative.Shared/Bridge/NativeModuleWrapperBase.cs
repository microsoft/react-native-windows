using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Collections.Generic;

namespace ReactNative.Bridge
{
    public abstract class NativeModuleWrapperBase<T> : INativeModule, INativeModuleWrapper
        where T : INativeModule
    {
        public NativeModuleWrapperBase(INativeModule module)
        {
            Module = module;
        }

        public bool CanOverrideExistingModule
        {
            get
            {
                return Module.CanOverrideExistingModule;
            }
        }

        public IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return Module.Constants;
            }
        }

        public abstract IReadOnlyDictionary<string, INativeMethod> Methods { get; }

        public INativeModule Module { get; }

        public Type ModuleType
        {
            get
            {
                return typeof(T);
            }
        }

        public string Name
        {
            get
            {
                return Module.Name;
            }
        }

        public void Initialize()
        {
            Module.Initialize();
        }

        public void OnReactInstanceDispose()
        {
            Module.OnReactInstanceDispose();
        }

        protected static INativeMethod CreateAsyncMethod(Action<IReactInstance, JArray> action)
        {
            return new NativeMethod(ReactDelegateFactoryBase.AsyncMethodType, action);
        }

        protected static INativeMethod CreatePromiseMethod(Action<IReactInstance, JArray> action)
        {
            return new NativeMethod(ReactDelegateFactoryBase.PromiseMethodType, action);
        }

        protected static T[] ToValueArray<T>(JArray array)
        {
            var count = array.Count;
            var result = new T[count];
            for (var i = 0; i < count; ++i)
            {
                result[i] = array[i].Value<T>();
            }
            return result;
        }

        class NativeMethod : INativeMethod
        {
            private readonly Action<IReactInstance, JArray> _action;

            public NativeMethod(string type, Action<IReactInstance, JArray> action)
            {
                Type = type;
                _action = action;
            }

            public string Type { get; }

            public void Invoke(IReactInstance reactInstance, JArray arguments)
            {
                _action(reactInstance, arguments);
            }
        }
    }
}
