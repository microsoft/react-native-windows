// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Class responsible for holding all <see cref="IJavaScriptModule"/>s
    /// registered to a <see cref="IReactInstance"/>. Requires that
    /// JavaScript modules use the <see cref="JavaScriptModuleBase"/> base
    /// class, and implement each of it's methods to dispatch through the
    /// <see cref="JavaScriptModuleBase.Invoke(object[], string)"/> method.
    /// </summary>
    public class JavaScriptModuleRegistry
    {
        private readonly IDictionary<Type, IJavaScriptModule> _moduleInstances;

        /// <summary>
        /// Instantiates the <see cref="JavaScriptModuleRegistry"/>. 
        /// </summary>
        public JavaScriptModuleRegistry()
        {
            _moduleInstances = new Dictionary<Type, IJavaScriptModule>();
        }

        /// <summary>
        /// Gets an instance of a <see cref="IJavaScriptModule"/>.
        /// </summary>
        /// <typeparam name="T">Type of JavaScript module.</typeparam>
        /// <param name="instance">The React instance.</param>
        /// <returns>The JavaScript module instance.</returns>
        public T GetJavaScriptModule<T>(IReactInstance instance) where T : IJavaScriptModule, new()
        {
            lock (_moduleInstances)
            {
                if (!_moduleInstances.TryGetValue(typeof(T), out var moduleInstance))
                {
                    var type = typeof(T);
                    moduleInstance = new T();
                    var invokeHandler = new JavaScriptModuleInvocationHandler(instance, type.Name);
                    moduleInstance.InvocationHandler = invokeHandler;
                    _moduleInstances.Add(type, moduleInstance);
                }

                return (T)moduleInstance;
            }
        }

        class JavaScriptModuleInvocationHandler : IInvocationHandler
        {
            private readonly IReactInstance _reactInstance;
            private readonly string _moduleName;

            public JavaScriptModuleInvocationHandler(IReactInstance reactInstance, string moduleName)
            {
                _reactInstance = reactInstance;
                _moduleName = moduleName;
            }

            public void Invoke(string name, object[] args)
            {
                _reactInstance.InvokeFunction(
                    _moduleName,
                    name,
                    JArray.FromObject(args),
                    Invariant($"JSCall__{_moduleName}_{name}"));
            }
        }
    }
}
