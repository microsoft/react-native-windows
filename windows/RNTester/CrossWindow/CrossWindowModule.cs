using ReactNative;
using ReactNative.Bridge;
using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace RNTesterApp
{
    /// <summary>
    /// A module that allows JS to share data.
    /// </summary>
    class CrossWindowModule : NativeModuleBase
    {
        /// <summary>
        /// Instantiates the <see cref="CrossWindowModule"/>.
        /// </summary>
        internal CrossWindowModule()
        {

        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "CrossWindow";
            }
        }

        [ReactMethod]
        public async void newWindow()
        {
            await openNewWindow(((App)(Application.Current)).Host.MainComponentName);
        }

        [ReactMethod]
        public async void runStressOpenCloseTest(string appName, IPromise promise)
        {
            int currentDelay = 4000;
            for (int i = 0; i < 30; i++)
            {
                currentDelay /= 2;

                // Create a window
                var dispatcher = await openNewWindow(appName);

                await Task.Delay(currentDelay);

                await await DispatcherHelpers.CallOnDispatcher(dispatcher, async () => await CloseWindow());
            }

            promise.Resolve(true);
        }

        private async Task<CoreDispatcher> openNewWindow(string reactApp)
        {
            var dispatcherTaskTask = DispatcherHelpers.CallOnDispatcher(async () =>
            {
                CoreApplicationView newView = CoreApplication.CreateNewView();
                int newViewId = 0;
                var dispatcher = newView.Dispatcher;

                await DispatcherHelpers.CallOnDispatcher(dispatcher, () =>
                {
                    Frame frame = new Frame();

                    Window.Current.Content = frame;

                    var host = ((App)(Application.Current)).Host;

                    frame.Content = new Page
                    {
                        Content = host.OnCreate(reactApp, null),
                    };

                    ApplicationView.GetForCurrentView().Consolidated += OnConsolidated;

                    // You have to activate the window in order to show it later.
                    Window.Current.Activate();

                    newViewId = ApplicationView.GetForCurrentView().Id;

                    return true;
                });

                bool viewShown = await ApplicationViewSwitcher.TryShowAsStandaloneAsync(newViewId);

                return dispatcher;
            });

            return await dispatcherTaskTask.Unwrap();
        }

        private async void OnConsolidated(ApplicationView appView, ApplicationViewConsolidatedEventArgs consolidatedArgs)
        {
            await CloseWindow();
        }

        private async Task CloseWindow()
        {
            // Cleanup the root view and close the Window
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
        }
    }
}
