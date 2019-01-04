// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge.Queue;
using ReactNative.Common;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using static System.FormattableString;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A higher level API on top of the <see cref="IJavaScriptExecutor" /> and module registries. This provides an
    /// environment allowing the invocation of JavaScript methods.
    /// </summary>
    class ReactInstance : IReactInstance
    {
        private readonly NativeModuleRegistry _registry;
        private readonly JavaScriptModuleRegistry _jsRegistry;
        private readonly Func<IJavaScriptExecutor> _jsExecutorFactory;
        private readonly JavaScriptBundleLoader _bundleLoader;

        private IReactBridge _bridge;

        private bool _initialized;

        private ReactInstance(
            IReactQueueConfiguration queueConfiguration,
            Func<IJavaScriptExecutor> jsExecutorFactory,
            NativeModuleRegistry registry,
            JavaScriptBundleLoader bundleLoader)
        {
            _registry = registry;
            _jsExecutorFactory = jsExecutorFactory;
            _jsRegistry = new JavaScriptModuleRegistry();
            _bundleLoader = bundleLoader;
            QueueConfiguration = queueConfiguration;
        }

        public bool IsDisposed
        {
            get;
            private set;
        }

        public IEnumerable<INativeModule> NativeModules
        {
            get
            {
                return _registry.Modules;
            }
        }

        public IReactQueueConfiguration QueueConfiguration
        {
            get;
        } 

        public T GetJavaScriptModule<T>() where T : IJavaScriptModule, new()
        {
            return _jsRegistry.GetJavaScriptModule<T>(this);
        }

        public T GetNativeModule<T>() where T : INativeModule
        {
            return _registry.GetModule<T>();
        }

        public async Task InitializeAsync()
        {
            DispatcherHelpers.AssertOnDispatcher();
            if (_initialized)
            {
                throw new InvalidOperationException("This React instance has already been initialized.");
            }

            _initialized = true;
            await QueueConfiguration.NativeModulesQueue.RunAsync(_registry.NotifyReactInstanceInitializeAsync).Unwrap();
        }

        public async Task InitializeBridgeAsync(CancellationToken token)
        {
            await _bundleLoader.InitializeAsync(token).ConfigureAwait(false);

            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "initializeBridge").Start())
            {
                await QueueConfiguration.JavaScriptQueue.RunAsync(() =>
                {
                    QueueConfiguration.JavaScriptQueue.AssertOnThread();

                    var jsExecutor = _jsExecutorFactory();
                    
                    using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "ReactBridgeCtor").Start())
                    {
                        _bridge = new ReactBridge(
                            jsExecutor,
                            new NativeModulesReactCallback(this),
                            QueueConfiguration.NativeModulesQueue);
                    }

                    using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "setBatchedBridgeConfig").Start())
                    {
                        _bridge.SetGlobalVariable("__fbBatchedBridgeConfig", BuildModulesConfig());
                    }

                    _bundleLoader.LoadScript(_bridge);
                }).ConfigureAwait(false);
            }
        }

        public void InvokeCallback(int callbackId, JArray arguments)
        {
            if (IsDisposed)
            {
                RnLog.Warn(ReactConstants.RNW, $"Invoking JS callback after bridge has been destroyed.");
                return;
            }

            QueueConfiguration.JavaScriptQueue.Dispatch(() =>
            {
                QueueConfiguration.JavaScriptQueue.AssertOnThread();
                if (IsDisposed)
                {
                    return;
                }

                using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "<callback>").Start())
                {
                    _bridge.InvokeCallback(callbackId, arguments);
                }
            });
        }

        public /* TODO: internal? */ void InvokeFunction(string module, string method, JArray arguments, string tracingName)
        {
            QueueConfiguration.JavaScriptQueue.Dispatch(() =>
            {
                QueueConfiguration.JavaScriptQueue.AssertOnThread();

                if (IsDisposed)
                {
                    return;
                }

                using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, tracingName).Start())
                {
                    if (_bridge == null)
                    {
                        RnLog.Error(ReactConstants.RNW, $"Invoking JS callback before bridge has been initialized. tracingName:{tracingName}.");
                        throw new InvalidOperationException($"Bridge has not been initialized. tracingName:{tracingName}.");
                    }

                    _bridge.CallFunction(module, method, arguments);
                }
            });
        }

        public async Task DisposeAsync()
        {
            DispatcherHelpers.AssertOnDispatcher();

            if (IsDisposed)
            {
                return;
            }

            IsDisposed = true;

            await QueueConfiguration.NativeModulesQueue.RunAsync(_registry.NotifyReactInstanceDisposeAsync).Unwrap().ConfigureAwait(false);
            await QueueConfiguration.JavaScriptQueue.RunAsync(() => _bridge?.Dispose()).ConfigureAwait(false);
            QueueConfiguration.Dispose();
        }

        private string BuildModulesConfig()
        {
            var stringWriter = new StringWriter(CultureInfo.InvariantCulture);
            try
            {
                using (var writer = new JsonTextWriter(stringWriter))
                {
                    writer.WriteStartObject();
                    writer.WritePropertyName("remoteModuleConfig");
                    _registry.WriteModuleDescriptions(writer);
                    writer.WriteEndObject();
                }

                return stringWriter.ToString();
            }
            finally
            {
                if (stringWriter != null)
                {
                    stringWriter.Dispose();
                }
            }
        }

        public sealed class Builder
        {
            private IReactQueueConfiguration _reactQueueConfiguration;
            private NativeModuleRegistry _registry;
            private Func<IJavaScriptExecutor> _jsExecutorFactory;
            private JavaScriptBundleLoader _bundleLoader;

            public IReactQueueConfiguration QueueConfiguration
            {
                set
                {
                    _reactQueueConfiguration = value;
                }
            }

            public NativeModuleRegistry Registry
            {
                set
                {
                    _registry = value;
                }
            }

            public Func<IJavaScriptExecutor> JavaScriptExecutorFactory
            {
                set
                {
                    _jsExecutorFactory = value;
                }
            }

            public JavaScriptBundleLoader BundleLoader
            {
                set
                {
                    _bundleLoader = value;
                }
            }

            public ReactInstance Build()
            {
                AssertNotNull(_reactQueueConfiguration, nameof(QueueConfiguration));
                AssertNotNull(_jsExecutorFactory, nameof(JavaScriptExecutorFactory));
                AssertNotNull(_registry, nameof(Registry));
                AssertNotNull(_bundleLoader, nameof(BundleLoader));
                 
                return new ReactInstance(
                    _reactQueueConfiguration,
                    _jsExecutorFactory,
                    _registry,
                    _bundleLoader);
            }

            private void AssertNotNull(object value, string name)
            {
                if (value == null)
                    throw new InvalidOperationException(Invariant($"'{name}' has not been set."));
            }
        }

        class NativeModulesReactCallback : IReactCallback
        {
            private readonly ReactInstance _parent;

            public NativeModulesReactCallback(ReactInstance parent)
            {
                _parent = parent;
            }

            public void Invoke(int moduleId, int methodId, JArray parameters)
            {
                _parent.QueueConfiguration.NativeModulesQueue.AssertOnThread();

                if (_parent.IsDisposed)
                {
                    return;
                }

                _parent._registry.Invoke(_parent.InvokeCallback, moduleId, methodId, parameters);
            }

            public JToken InvokeSync(int moduleId, int methodId, JArray parameters)
            {
                _parent.QueueConfiguration.JavaScriptQueue.AssertOnThread();

                if (_parent.IsDisposed)
                {
                    return null;
                }

                return _parent._registry.InvokeSync(_parent.InvokeCallback, moduleId, methodId, parameters);
            }

            public void OnBatchComplete()
            {
                _parent.QueueConfiguration.NativeModulesQueue.AssertOnThread();

                // The bridge may have been destroyed due to an exception
                // during the batch. In that case native modules could be in a
                // bad state so we don't want to call anything on them.
                if (!_parent.IsDisposed)
                {
                    using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "OnBatchComplete").Start())
                    {
                        _parent._registry.OnBatchComplete();
                    }
                }
            }
        }
    }
}
