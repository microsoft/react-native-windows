// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.Bridge.Queue;
using ReactNative.Modules.Core;
using ReactNative.Modules.DeviceInfo;
using ReactNative.Modules.DevSupport;
using ReactNative.Modules.SystemInfo;
using ReactNative.Tracing;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;

namespace ReactNative
{
    /// <summary>
    /// Package defining core framework modules (e.g., <see cref="UIManagerModule"/>). 
    /// It should be used for modules that require special integration with
    /// other framework parts (e.g., with the list of packages to load view
    /// managers from).
    /// </summary>
    class CoreModulesPackage : IReactPackage
    {
        private readonly ReactInstanceManager _reactInstanceManager;
        private readonly Action _hardwareBackButtonHandler;
        private readonly UIImplementationProvider _uiImplementationProvider;
        private readonly bool _lazyViewManagersEnabled;

        public CoreModulesPackage(
            ReactInstanceManager reactInstanceManager,
            Action hardwareBackButtonHandler,
            UIImplementationProvider uiImplementationProvider,
            bool lazyViewManagersEnabled)
        {
            _reactInstanceManager = reactInstanceManager;
            _hardwareBackButtonHandler = hardwareBackButtonHandler;
            _uiImplementationProvider = uiImplementationProvider;
            _lazyViewManagersEnabled = lazyViewManagersEnabled;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope", Justification = "Caller manages scope of returned list of disposables.")]
        public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
        {
            var uiManagerModule = default(INativeModule);
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "createUIManagerModule").Start())
            {
                var layoutActionQueue = new LayoutActionQueue(reactContext.HandleException);
                var options = _lazyViewManagersEnabled ? UIManagerModuleOptions.LazyViewManagers : UIManagerModuleOptions.None;
                uiManagerModule = new UIManagerModule(
                    reactContext,
                    _reactInstanceManager.CreateAllViewManagers(reactContext),
                    _uiImplementationProvider,
                    layoutActionQueue,
                    options);
            }

            return new List<INativeModule>
            {
                //new AnimationsDebugModule(
                //    reactContext,
                //    _reactInstanceManager.DevSupportManager.DevSettings),
                new DeviceEventManagerModule(reactContext, _hardwareBackButtonHandler),
                new DeviceInfoModule(reactContext),
                new ExceptionsManagerModule(_reactInstanceManager.DevSupportManager),
                new PlatformConstantsModule(),
                new SourceCodeModule(
                    _reactInstanceManager.SourceUrl,
                    _reactInstanceManager.DevSupportManager.SourceMapUrl),
                new Timing(reactContext),
                uiManagerModule,
                //new DebugComponentOwnershipModule(reactContext),
            };
        }

        public IReadOnlyList<IViewManager> CreateViewManagers(
            ReactContext reactContext)
        {
            return new List<IViewManager>(0);
        }
    }
}
