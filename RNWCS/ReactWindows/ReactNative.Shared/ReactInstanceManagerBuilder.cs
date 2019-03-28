// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.Chakra.Executor;
using ReactNative.Common;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;

namespace ReactNative
{
    /// <summary>
    /// Builder for <see cref="ReactInstanceManager"/>. 
    /// </summary>
    public class ReactInstanceManagerBuilder
    {
        private readonly List<IReactPackage> _packages = new List<IReactPackage>();
        private string _jsBundleFile;
        private string _jsMainModuleName;
        private bool _useDeveloperSupport;
        private LifecycleState? _initialLifecycleState;
        private UIImplementationProvider _uiImplementationProvider;
        private Action<Exception> _nativeModuleCallExceptionHandler;
        private Func<IJavaScriptExecutor> _jsExecutorFactory;
        private bool _lazyViewManagersEnabled;

        /// <summary>
        /// Sets a provider of <see cref="UIImplementation"/>.
        /// </summary>
        /// <remarks>
        /// Uses default provider if null is passed.
        /// </remarks>
        public UIImplementationProvider UIImplementationProvider
        {
            set
            {
                _uiImplementationProvider = value;
            }
        }

        /// <summary>
        /// Path to the JavaScript bundle file to be loaded from the file system.
        /// </summary>
        public string JavaScriptBundleFile
        {
            set
            {
                _jsBundleFile = value;
            }
        }

        /// <summary>
        /// Path to your app's main module on the packager server. This is used
        /// when loading JavaScript during development. All paths are
        /// relative to the root folder the packager is service files from.
        /// </summary>
        /// <remarks>
        /// Examples:
        /// <code>"index.windows"</code>
        /// <code>"subdirectory/index.windows"</code>
        /// </remarks>
        public string JavaScriptMainModuleName
        {
            set
            {
                _jsMainModuleName = value;
            }
        }

        /// <summary>
        /// The <see cref="IReactPackage"/> instances to load modules and view
        /// managers from.
        /// </summary>
        public List<IReactPackage> Packages
        {
            get
            {
                return _packages;
            }
        }

        /// <summary>
        /// When <code>true</code>, developer options such as JavaScript
        /// reloading and debugging are enabled.
        /// </summary>
        public bool UseDeveloperSupport
        {
            set
            {
                _useDeveloperSupport = value;
            }
        }

        /// <summary>
        /// Sets the initial lifecycle state of the host.
        /// </summary>
        public LifecycleState InitialLifecycleState
        {
            set
            {
                _initialLifecycleState = value;
            }
        }

        /// <summary>
        /// Set the exception handler for all native module calls. If not set,
        /// the default behavior shows a redbox in dev mode, or rethrows
        /// (crashing the app) in release mode.
        /// </summary>
        public Action<Exception> NativeModuleCallExceptionHandler
        {
            set
            {
                _nativeModuleCallExceptionHandler = value;
            }
        }

        /// <summary>
        /// When <code>true</code>, view manager constants, including custom
        /// events and native props configuration are loaded
        /// on-demand rather than at startup.
        /// </summary>
        public bool LazyViewManagersEnabled
        {
            set
            {
                _lazyViewManagersEnabled = value;
            }
        }

        /// <summary>
        /// Sets the JavaScript executor factory.
        /// </summary>
        public Func<IJavaScriptExecutor> JavaScriptExecutorFactory
        {
            set
            {
                _jsExecutorFactory = value;
            }
        }

        /// <summary>
        /// Instantiates a new <see cref="ReactInstanceManager"/>. 
        /// </summary>
        /// <returns>The instance.</returns>
        public ReactInstanceManager Build()
        {
            if (!_initialLifecycleState.HasValue)
            {
                throw new InvalidOperationException("Initial lifecycle state was not set.");
            }

            if (!_useDeveloperSupport && _jsBundleFile == null)
            {
                throw new InvalidOperationException(
                    "JavaScriptBundleFile must be provided when UseDeveloperSupport is false.");
            }

            if (_jsBundleFile == null && _jsMainModuleName == null)
            {
                throw new InvalidOperationException("Either MainModulePath or the JavaScriptBundleFile must be provided.");
            }

            if (_uiImplementationProvider == null)
            {
                _uiImplementationProvider = new UIImplementationProvider();
            }

            if (_jsExecutorFactory == null)
            {
                _jsExecutorFactory = () => new ChakraJavaScriptExecutor();
            }

            return new ReactInstanceManager(
                _jsBundleFile,
                _jsMainModuleName,
                _packages,
                _useDeveloperSupport,
                _initialLifecycleState.Value,
                _uiImplementationProvider,
                _jsExecutorFactory,
                _nativeModuleCallExceptionHandler,
                _lazyViewManagersEnabled);
        }
    }
}
