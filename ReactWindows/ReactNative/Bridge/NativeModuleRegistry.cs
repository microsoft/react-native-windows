using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using static System.FormattableString;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A set of native APIs exposed to a particular JavaScript instance.
    /// </summary>
    public sealed class NativeModuleRegistry
    {
        private readonly IReadOnlyList<ModuleDefinition> _moduleTable;
        private readonly IReadOnlyDictionary<Type, INativeModule> _moduleInstances;
        private readonly IList<IOnBatchCompleteListener> _batchCompleteListenerModules;

        private NativeModuleRegistry(
            IReadOnlyList<ModuleDefinition> moduleTable,
            IReadOnlyDictionary<Type, INativeModule> moduleInstances)
        {
            _moduleTable = moduleTable;
            _moduleInstances = moduleInstances;
            _batchCompleteListenerModules = _moduleTable
                .Select(moduleDefinition => moduleDefinition.Target)
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
            var instance = default(INativeModule);
            if (_moduleInstances.TryGetValue(typeof(T), out instance))
            {
                return (T)instance;
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
                module.OnBatchComplete();
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
        /// <param name="reactInstance">The React instance.</param>
        /// <param name="moduleId">The module ID.</param>
        /// <param name="methodId">The method ID.</param>
        /// <param name="parameters">The parameters.</param>
        internal void Invoke(
            IReactInstance reactInstance,
            int moduleId,
            int methodId,
            JArray parameters)
        {
            if (moduleId < 0)
                throw new ArgumentOutOfRangeException(nameof(moduleId), "Invalid module ID: " + moduleId);
            if (_moduleTable.Count < moduleId)
                throw new ArgumentOutOfRangeException(nameof(moduleId), "Call to unknown module: " + moduleId);

            _moduleTable[moduleId].Invoke(reactInstance, methodId, parameters);
        }

        /// <summary>
        /// Hook to notify modules that the <see cref="IReactInstance"/> has
        /// been initialized.
        /// </summary>
        internal void NotifyReactInstanceInitialize()
        {
            DispatcherHelpers.AssertOnDispatcher();
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "NativeModuleRegistry_NotifyReactInstanceInitialize").Start())
            {
                foreach (var module in _moduleInstances.Values)
                {
                    module.Initialize();
                }
            }
        }

        /// <summary>
        /// Hook to notify modules that the <see cref="IReactInstance"/> has
        /// been disposed.
        /// </summary>
        internal void NotifyReactInstanceDispose()
        {
            DispatcherHelpers.AssertOnDispatcher();
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "NativeModuleRegistry_NotifyReactInstanceDestroy").Start())
            {
                foreach (var module in _moduleInstances.Values)
                {
                    module.OnReactInstanceDispose();
                }
            }
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

            public void Invoke(IReactInstance reactInstance, int methodId, JArray parameters)
            {
                var method = _methods[methodId];
                using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, method.TracingName).Start())
                {
                    method.Method.Invoke(reactInstance, parameters);
                }
            }

            public void WriteModuleDescription(JsonWriter writer)
            {
                writer.WriteStartArray();
                writer.WriteValue(Name);
                JObject.FromObject(Target.Constants).WriteTo(writer);

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

                var existing = default(INativeModule);
                if (_modules.TryGetValue(module.Name, out existing) && !module.CanOverrideExistingModule)
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

                var idx = 0;
                foreach (var module in _modules.Values)
                {
                    var moduleDef = new ModuleDefinition(module.Name, module);
                    moduleTable.Add(moduleDef);
                    moduleInstances.Add(module.GetType(), module);
                }

                return new NativeModuleRegistry(moduleTable, moduleInstances);
            }
        }
    }
}
