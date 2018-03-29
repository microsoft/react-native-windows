#if RNCODEGEN
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace ReactNative.CodeGen
{
    class MetadataExtractorReactPackage : IReactPackage
    {
        private readonly IReactPackage _reactPackage;

        public MetadataExtractorReactPackage(IReactPackage reactPackage)
        {
            _reactPackage = reactPackage;
        }

        public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
        {
            var nativeModules = _reactPackage.CreateNativeModules(reactContext);

            var nativeModuleMetadata = new JArray();
            var metadata = new JObject
            {
                { "name", _reactPackage.GetType().FullName },
                { "modules", nativeModuleMetadata },
            };

            foreach (var nativeModule in nativeModules)
            {
                nativeModuleMetadata.Add(GetNativeModuleMetadata(nativeModule));
            }

            MetadataWriter.Write(metadata);

            return nativeModules;
        }

        public IReadOnlyList<IViewManager> CreateViewManagers(ReactContext reactContext)
        {
            return _reactPackage.CreateViewManagers(reactContext);
        }

        private JObject GetNativeModuleMetadata(INativeModule nativeModule)
        {
            var type = nativeModule.GetType();
            var methodMetadata = new JArray();
            var metadata = new JObject
            {
                { "name", type.FullName },
                { "methods", methodMetadata },
            };

            var reactMethods = from m in type.GetMethods()
                               let a = m.GetCustomAttribute<ReactMethodAttribute>()
                               where a != null
                               select new { Method = m, Attribute = a };

            foreach (var reactMethod in reactMethods)
            {
                // TODO: validate methods
                var parameterMetadata = new JArray();
                foreach (var parameter in reactMethod.Method.GetParameters())
                {
                    parameterMetadata.Add(GetParameterMetadata(parameter.ParameterType));
                }

                methodMetadata.Add(new JObject
                {
                    { "name", reactMethod.Method.Name },
                    { "type", GetMethodType(reactMethod.Method, reactMethod.Attribute) },
                    { "parameters", parameterMetadata },
                });
            }

            return metadata;
        }

        private static JObject GetParameterMetadata(Type parameterType)
        {
            var parameterTypeInfo = parameterType.GetTypeInfo();
            if (typeof(JToken).IsAssignableFrom(parameterType))
            {
                return new JObject
                {
                    { "kind", "json" },
                    { "type", parameterType.FullName },
                };
            }
            else if (parameterType == typeof(ICallback))
            {
                return new JObject
                {
                    { "kind", "callback" },
                };
            }
            else if (parameterType == typeof(IPromise))
            {
                return new JObject
                {
                    { "kind", "promise" },
                };
            }
            else if (parameterTypeInfo.IsGenericType
                && parameterTypeInfo.GetGenericTypeDefinition() == typeof(Nullable<>))
            {
                return new JObject
                {
                    { "kind", "nullable" },
                    { "type", parameterType.GetGenericArguments()[0].FullName },
                };
            }

            // TODO: replace with minimal reflection
            // E.g., deconstruct arrays, primitives, and nullables
            return new JObject
            {
                { "kind", "value" },
                { "type", parameterType.FullName },
            };
        }

        private static string GetMethodType(MethodInfo method, ReactMethodAttribute attribute)
        {
            if (attribute.IsBlockingSynchronousMethod)
            {
                return "sync";
            }

            var parameters = method.GetParameters();
            if (parameters.Length > 0 && parameters.Last().ParameterType == typeof(IPromise))
            {
                return "promise";
            }

            return "async";
        }
    }
}
#endif
