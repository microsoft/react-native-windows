using ReactNative;
using ReactNative.Bridge;
using System;
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
            await CoreApplication.MainView.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                CoreApplicationView newView = CoreApplication.CreateNewView();
                int newViewId = 0;
                await newView.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    Frame frame = new Frame();

                    Window.Current.Content = frame;

                    var host = ((App)(Application.Current)).Host;

                    frame.Content = new Page
                    {
                        Content = host.OnCreate(),
                    };

                    ApplicationView.GetForCurrentView().Consolidated += OnConsolidated;

                    // You have to activate the window in order to show it later.
                    Window.Current.Activate();

                    newViewId = ApplicationView.GetForCurrentView().Id;
                });
                bool viewShown = await ApplicationViewSwitcher.TryShowAsStandaloneAsync(newViewId);
            });
        }

        private async void OnConsolidated(ApplicationView appView, ApplicationViewConsolidatedEventArgs consolidatedArgs)
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
