// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge.Queue;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;
using static System.FormattableString;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A set of native APIs exposed to a particular JavaScript instance.
    /// </summary>
    public sealed class NativeModuleRegistry
    {
        private readonly ReactContext _reactContext;
        private readonly IReadOnlyList<ModuleDefinition> _moduleTable;
        private readonly IReadOnlyDictionary<Type, INativeModule> _moduleInstances;
        private readonly IList<IOnBatchCompleteListener> _batchCompleteListenerModules;

        private NativeModuleRegistry(
            ReactContext reactContext,
            IReadOnlyList<ModuleDefinition> moduleTable,
            IReadOnlyDictionary<Type, INativeModule> moduleInstances)
        {
            _reactContext = reactContext;
            _moduleTable = moduleTable;
            _moduleInstances = moduleInstances;
            _batchCompleteListenerModules = _moduleTable
                .Select(moduleDefinition =>
                    moduleDefinition.Target is INativeModuleWrapper wrapper
                        ? wrapper.Module
                        : moduleDefinition.Target)
                .OfType<IOnBatchCompleteListener>()
                .ToList();
        }

        /// <summary>
        /// The set of native modules exposed.
        /// </summary>
        public IEnumerable<INativeModule> Modules
        {
            get
            {
                return _moduleInstances.Values;
            }
        }

        /// <summary>
        /// Gets a module instance of a specific type.
        /// </summary>
        /// <typeparam name="T">Type of module instance.</typeparam>
        /// <returns>The module instance.</returns>
        public T GetModule<T>() where T : INativeModule
        {
            if (_moduleInstances.TryGetValue(typeof(T), out var instance))
            {
                var wrapper = instance as INativeModuleWrapper;
                return wrapper != null ? (T)wrapper.Module : (T)instance;
            }

            throw new InvalidOperationException("No module instance for type '{0}'.");
        }

        /// <summary>
        /// Triggers the batch completion event for all modules.
        /// </summary>
        public void OnBatchComplete()
        {
            foreach (var module in _batchCompleteListenerModules)
            {
                Dispatch((INativeModule)module, module.OnBatchComplete);
            }
        }

        /// <summary>
        /// Write the module descriptions to the given <see cref="JsonWriter"/>.
        /// </summary>
        /// <param name="writer">The JSON writer.</param>
        internal void WriteModuleDescriptions(JsonWriter writer)
        {
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "CreateJSON").Start())
            {
                writer.WriteStartArray();
                foreach (var moduleDef in _moduleTable)
                {
                    moduleDef.WriteModuleDescription(writer);
                }
                writer.WriteEndArray();
            }
        }

        /// <summary>
        /// Invoke a method on a native module.
        /// </summary>
        /// <param name="invokeCallback">The invoke callback delegate.</param>
        /// <param name="moduleId">The module ID.</param>
        /// <param name="methodId">The method ID.</param>
        /// <param name="parameters">The parameters.</param>
        internal void Invoke(
            InvokeCallback invokeCallback,
            int moduleId,
            int methodId,
            JArray parameters)
        {
            if (moduleId < 0)
                throw new ArgumentOutOfRangeException(nameof(moduleId), "Invalid module ID: " + moduleId);
            if (_moduleTable.Count < moduleId)
                throw new ArgumentOutOfRangeException(nameof(moduleId), "Call to unknown module: " + moduleId);

            var actionQueue = _moduleTable[moduleId].Target.ActionQueue;
            if (actionQueue != null)
            {
                actionQueue.Dispatch(() => _moduleTable[moduleId].Invoke(invokeCallback, methodId, parameters));
            }
            else
            {
                _moduleTable[moduleId].Invoke(invokeCallback, methodId, parameters);
            }
        }

        /// <summary>
        /// Invoke the native method synchronously.
        /// </summary>
        /// <param name="invokeCallback">The invoke callback delegate.</param>
        /// <param name="moduleId">The module ID.</param>
        /// <param name="methodId">The method ID.</param>
        /// <param name="parameters">The parameters.</param>
        /// <returns>The value returned from the method.</returns>
        internal JToken InvokeSync(
            InvokeCallback invokeCallback,
            int moduleId,
            int methodId,
            JArray parameters)
        {
            if (moduleId < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(moduleId), "Invalid module ID: " + moduleId);
            }

            if (_moduleTable.Count < moduleId)
            {
                throw new ArgumentOutOfRangeException(nameof(moduleId), "Call to unknown module: " + moduleId);
            }

            return _moduleTable[moduleId].Invoke(invokeCallback, methodId, parameters);
        }

        /// <summary>
        /// Hook to notify modules that the <see cref="IReactInstance"/> has
        /// been initialized.
        /// </summary>
        internal async Task NotifyReactInstanceInitializeAsync()
        {
            _reactContext.AssertOnNativeModulesQueueThread();
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "NativeModuleRegistry_NotifyReactInstanceInitialize").Start())
            {
                await Task.WhenAll(_moduleInstances.Values.Select(module => RunAsync(module, module.Initialize)));
            }
        }

        /// <summary>
        /// Hook to notify modules that the <see cref="IReactInstance"/> has
        /// been disposed.
        /// </summary>
        /// <returns>Awaitable task.</returns>
        internal async Task NotifyReactInstanceDisposeAsync()
        {
            _reactContext.AssertOnNativeModulesQueueThread();
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "NativeModuleRegistry_NotifyReactInstanceDestroy").Start())
            {
                await Task.WhenAll(_moduleInstances.Values.Select(DisposeModuleAsync));
            }
        }

        private static void Dispatch(INativeModule module, Action action)
        {
            // If the module has an action queue, dispatch there;
            // otherwise execute inline.
            if (module.ActionQueue != null)
            {
                module.ActionQueue.Dispatch(action);
            }
            else
            {
                action();
            }
        }

        private static async Task RunAsync(INativeModule module, Action action)
        {
            // If the module has an action queue, call there;
            // otherwise execute inline.
            if (module.ActionQueue != null)
            {
                await module.ActionQueue.RunAsync(action);
            }
            else
            {
                action();
            }
        }

        private static Task DisposeModuleAsync(INativeModule module)
        {
            return module.ActionQueue != null
                ? DisposeModuleOnActionQueueAsync(module)
                : module.DisposeAsync();
        }
        private static async Task DisposeModuleOnActionQueueAsync(INativeModule module)
        {
            await module.ActionQueue.RunAsync(module.DisposeAsync).Unwrap();
            module.ActionQueue.Dispose();
        }

        class ModuleDefinition
        {
            private readonly IList<MethodRegistration> _methods;

            public ModuleDefinition(string name, INativeModule target)
            {
                Name = name;
                Target = target;
                _methods = new List<MethodRegistration>(target.Methods.Count);

                foreach (var entry in target.Methods)
                {
                    _methods.Add(
                        new MethodRegistration(
                            entry.Key,
                            "NativeCall__" + target.Name + "_" + entry.Key,
                            entry.Value));
                }
            }

            public int Id { get; }

            public string Name { get; }

            public INativeModule Target { get; }

            public JToken Invoke(InvokeCallback invokeCallback, int methodId, JArray parameters)
            {
                var method = _methods[methodId];
                using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, method.TracingName).Start())
                {
                    return method.Method.Invoke(invokeCallback, parameters);
                }
            }

            public void WriteModuleDescription(JsonWriter writer)
            {
                writer.WriteStartArray();
                writer.WriteValue(Name);
                Target.Constants.WriteTo(writer);

                if (_methods.Count > 0)
                {
                    var syncMethodIds = new List<int>();
                    var promiseMethodIds = new List<int>();
                    writer.WriteStartArray();
                    for (var i = 0; i < _methods.Count; ++i)
                    {
                        var method = _methods[i];
                        writer.WriteValue(method.Name);
                        if (method.Method.Type == ReactDelegateFactoryBase.PromiseMethodType)
                        {
                            promiseMethodIds.Add(i);
                        }
                        else if (method.Method.Type == ReactDelegateFactoryBase.SyncMethodType)
                        {
                            syncMethodIds.Add(i);
                        }
                    }
                    writer.WriteEndArray();

                    if (promiseMethodIds.Count > 0 || syncMethodIds.Count > 0)
                    {
                        WriteList(writer, promiseMethodIds);
                        if (syncMethodIds.Count > 0)
                        {
                            WriteList(writer, syncMethodIds);
                        }
                    }
                }

                writer.WriteEndArray();
            }

            private static void WriteList(JsonWriter writer, IList<int> values)
            {
                writer.WriteStartArray();
                for (var i = 0; i < values.Count; ++i)
                {
                    writer.WriteValue(values[i]);
                }
                writer.WriteEndArray();
            }

            class MethodRegistration
            {
                public MethodRegistration(string name, string tracingName, INativeMethod method)
                {
                    Name = name;
                    TracingName = tracingName;
                    Method = method;
                }

                public string Name { get; }

                public string TracingName { get; }

                public INativeMethod Method { get; }
            }
        }

        /// <summary>
        /// Builder for <see cref="NativeModuleRegistry"/>.
        /// </summary>
        public sealed class Builder
        {
            private readonly IDictionary<string, INativeModule> _modules = 
                new Dictionary<string, INativeModule>();

            private readonly ReactContext _reactContext;

            /// <summary>
            /// Instantiates the <see cref="Builder"/>.
            /// </summary>
            /// <param name="reactContext">The React context.</param>
            public Builder(ReactContext reactContext)
            {
                if (reactContext == null)
                    throw new ArgumentNullException(nameof(reactContext));

                _reactContext = reactContext;
            }

            /// <summary>
            /// Add a native module to the builder.
            /// </summary>
            /// <param name="module">The native module.</param>
            /// <returns>The builder instance.</returns>
            public Builder Add(INativeModule module)
            {
                if (module == null)
                    throw new ArgumentNullException(nameof(module));
                if (module.Name == null)
                    throw new ArgumentException(
                        Invariant($"Native module '{module.GetType()}' cannot have a null `Name`."),
                        nameof(module));

                if (_modules.TryGetValue(module.Name, out var existing) && !module.CanOverrideExistingModule)
                {
                    throw new InvalidOperationException(
                        string.Format(
                            CultureInfo.InvariantCulture,
                            "Native module '{0}' tried to override '{1}' for module name '{2}'. " +
                            "If this was your intention, override `CanOverrideExistingModule`.",
                            module.GetType().Name,
                            existing.GetType().Name,
                            module.Name));

                }

                _modules[module.Name] = module;

                return this;
            }

            /// <summary>
            /// Build a <see cref="NativeModuleRegistry"/> instance.
            /// </summary>
            /// <returns>The instance.</returns>
            public NativeModuleRegistry Build()
            {
                var moduleTable = new List<ModuleDefinition>(_modules.Count); 
                var moduleInstances = new Dictionary<Type, INativeModule>(_modules.Count);

                foreach (var module in _modules.Values)
                {
                    var name = module.Name;
                    var moduleDef = new ModuleDefinition(name, module);
                    moduleTable.Add(moduleDef);
                    var wrapper = module as INativeModuleWrapper;
                    var type = (wrapper?.Module ?? module).GetType();
                    moduleInstances.Add(type, module);
                }

                return new NativeModuleRegistry(_reactContext, moduleTable, moduleInstances);
            }
        }
    }
}
