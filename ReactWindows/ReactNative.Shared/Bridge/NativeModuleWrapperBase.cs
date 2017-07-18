using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Diagnostics;

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

        protected static bool ToBoolean(JToken token)
        {
            // TODO: use TypeAs check
            var value = (JValue)token;
            if (value.Type == JTokenType.Boolean)
            {
                return (bool)value.Value;
            }

            return Convert.ToBoolean(value.Value);
        }

        protected static int ToInt32(JToken token)
        {
            // TODO: use TypeAs check
            var value = (JValue)token;
            if (value.Type == JTokenType.Integer)
            {
                return (int)(long)value.Value;
            }
            else if (value.Type == JTokenType.Float)
            {
                return (int)(double)value.Value;
            }
            
            return Convert.ToInt32(value.Value);
        }

        protected static string ToString(JToken token)
        {
            // TODO: use TypeAs check
            var value = (JValue)token;
            if (value.Type == JTokenType.String)
            {
                return (string)value.Value;
            }

            return value.Value.ToString();
        }

        protected static TValue[] ToValueArray<TValue>(JToken token)
        {
            return ToValueArray(token, t => t.Value<TValue>());
        }

        protected static TValue[] ToValueArray<TValue>(JToken token, Func<JToken, TValue> convert)
        {
            // TODO: use TypeAs check
            var array = (JArray)token;
            var count = array.Count;
            var result = new TValue[count];
            for (var i = 0; i < count; ++i)
            {
                result[i] = convert(array[i]);
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
