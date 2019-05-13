// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Tests
{
    [TestClass]
    public class ReactRootViewTests
    {
        [TestMethod]
        public async Task ReactRootView_SecondaryWindow()
        {
            var jsBundleFile = "ms-appx:///Resources/mwtest.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(
                () => manager.CreateReactContextAsync(CancellationToken.None));

            var dispatcher = await CreateView(() =>
            {
                var rv = new ReactRootView();
                rv.StartReactApplication(
                    manager,
                    "alt_window",
                    null);
                return rv;
            });

            await CloseView(manager, dispatcher);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactRootView_SecondaryWindowStress()
        {
            var jsBundleFile = "ms-appx:///Resources/mwtest.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(
                () => manager.CreateReactContextAsync(CancellationToken.None));

            int currentDelay = 2000;

            for (int i = 0; i < 30; i++)
            {
                currentDelay /= 2;

                // Create a window
                var dispatcher = await CreateView(() =>
                {
                    var rv = new ReactRootView();
                    rv.StartReactApplication(
                        manager,
                        "alt_window",
                        null);
                    return rv;
                });

                await Task.Delay(currentDelay);

                await CloseView(manager, dispatcher);
            }

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        private static ReactInstanceManager CreateReactInstanceManager(string jsBundleFile, LifecycleState initialLifecycleState = LifecycleState.Foreground)
        {
            ReactNative.Bridge.DispatcherHelpers.Initialize();
            ReactChoreographer.Initialize();

            return new ReactInstanceManagerBuilder
            {
                InitialLifecycleState = initialLifecycleState,
                JavaScriptBundleFile = jsBundleFile,
            }.Build();
        }

        private static async Task DisposeInstanceManager(ReactInstanceManager manager)
        {
            await manager.DisposeAsync();

            // Go back to the initial state as set by the host test app
            ReactNative.Bridge.DispatcherHelpers.Initialize();
        }

        private static async Task<CoreDispatcher> CreateView(Func<ReactRootView> rootViewCreateAction)
        {
            var dispatcherTask = DispatcherHelpers.CallOnDispatcherAsync<CoreDispatcher>(async () =>
            {
                    CoreApplicationView newView = CoreApplication.CreateNewView();
                    int newViewId = 0;
                    var dispatcher = newView.Dispatcher;

                    await DispatcherHelpers.CallOnDispatcherAsync(dispatcher, () =>
                    {
                        Frame frame = new Frame();

                        Window.Current.Content = frame;

                        frame.Content = new Page
                        {
                            Content = rootViewCreateAction(),
                        };

                        // You have to activate the window in order to show it later.
                        Window.Current.Activate();

                        newViewId = ApplicationView.GetForCurrentView().Id;

                        return true;
                    });

                    bool viewShown = await ApplicationViewSwitcher.TryShowAsStandaloneAsync(newViewId);

                    return dispatcher;
            });

            return await dispatcherTask;
        }

        private static async Task CloseView(ReactInstanceManager manager, CoreDispatcher dispatcher)
        {
            await DispatcherHelpers.CallOnDispatcherAsync(dispatcher, async () =>
            {
                var window = Window.Current;
                if (window != null)
                {
                    var frame = window.Content as Frame;
                    if (frame != null)
                    {
                        var page = frame.Content as Page;
                        if (page != null)
                        {
                            var rootView = page.Content as ReactRootView;
                            if (rootView != null)
                            {
                                await rootView.StopReactApplicationAsync();

                                Window.Current.Close();
                                page.Content = null;
                            }
                        }
                    }
                }
            });
        }
    }
}
