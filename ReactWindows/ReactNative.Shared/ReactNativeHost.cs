// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ReactNative
{
    /// <summary>
    /// Simple class that holds the <see cref="ReactInstanceManager"/>. 
    /// </summary>
    public abstract class ReactNativeHost : IAsyncDisposable
    {
        private ReactInstanceManager _reactInstanceManager;

        /// <summary>
        /// Get the current <see cref="ReactInstanceManager"/> instance, or create one.
        /// </summary>
        public ReactInstanceManager ReactInstanceManager
        {
            get
            {
                if (_reactInstanceManager == null)
                {
                    _reactInstanceManager = CreateReactInstanceManager();
                }

                return _reactInstanceManager;
            }
        }

        /// <summary>
        /// Checks whether this host contains a <see cref="ReactInstanceManager"/>
        /// </summary>
        public bool HasInstance
        {
            get
            {
                return _reactInstanceManager != null;
            }
        }

        /// <summary>
        /// The main component name.
        /// </summary>
        public abstract string MainComponentName { get; }

        /// <summary>
        /// Signals whether developer mode should be enabled.
        /// </summary>
        public abstract bool UseDeveloperSupport { get; }

        /// <summary>
        /// Instantiates the JavaScript executor.
        /// </summary>
        protected virtual Func<IJavaScriptExecutor> JavaScriptExecutorFactory
        {
            get
            {
                return null;
            }
        }

        /// <summary>
        /// The name of the main module.
        /// </summary>
        /// <remarks>
        /// Determines the URL to fetch the JavaScript bundle from the packager
        /// server. It is only used when dev support is enabled.
        /// </remarks>
        protected virtual string JavaScriptMainModuleName
        {
            get
            {
                return "index.windows";
            }
        }

        /// <summary>
        /// The custom path of the bundle file.
        /// </summary>
        /// <remarks>
        /// This is used in cases where the bundle should be loaded from a
        /// custom path.
        /// </remarks>
        protected virtual string JavaScriptBundleFile
        {
            get
            {
                return null;
            }
        }

        /// <summary>
        /// When <code>true</code>, view manager constants, including custom
        /// events and native props configuration are loaded
        /// on-demand rather than at startup.
        /// </summary>
        protected virtual bool LazyViewManagersEnabled
        {
            get
            {
                return false;
            }
        }

        /// <summary>
        /// The list of <see cref="IReactPackage"/>s used by the application.
        /// </summary>
        protected abstract List<IReactPackage> Packages { get; }

        /// <summary>
        /// Creates a new root view.
        /// </summary>
        /// <remarks>
        /// Has to be called in the context of the dispatcher associated with the hosting view
        /// </remarks>
        /// <returns>The root view.</returns>
        public virtual ReactRootView CreateRootView()
        {
            return new ReactRootView();
        }

        /// <summary>
        /// Dispose the current instance and release the reference to it.
        /// </summary>
        /// <returns>
        /// A task to await the dispose operation.
        /// </returns>
        public async Task DisposeAsync()
        {
            if (_reactInstanceManager != null)
            {
                await _reactInstanceManager.DisposeAsync();
                _reactInstanceManager = null;
            }
        }

        private ReactInstanceManager CreateReactInstanceManager()
        {
            var builder = new ReactInstanceManagerBuilder
            {
                UseDeveloperSupport = UseDeveloperSupport,
                InitialLifecycleState = LifecycleState.BeforeCreate,
                JavaScriptBundleFile = JavaScriptBundleFile,
                JavaScriptMainModuleName = JavaScriptMainModuleName,
                JavaScriptExecutorFactory = JavaScriptExecutorFactory,
                LazyViewManagersEnabled = LazyViewManagersEnabled,
            };

            builder.Packages.AddRange(Packages);
            return builder.Build();
        }
    }
}
