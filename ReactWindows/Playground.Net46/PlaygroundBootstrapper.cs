using ReactNative;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace Playground.Net46
{
    public sealed class PlaygroundBootstrapper : ReactNativeBootstrapper
    {
        #region Protected Virtual Methods

        protected override ReactPage CreateRootPage()
        {
            return new AppReactPage();
        }

        //protected override void InitializeRootPage()
        //{
        //    base.InitializeRootPage();
        //}

        protected override DependencyObject CreateShell()
        {
            return new MainWindow();
        }

        protected override void InitializeShell()
        {
            if (this.RootPage == null)
            {
                throw new InvalidOperationException("RootPage is null");
            }

            var shellWindow = this.Shell as Window;

            if (shellWindow == null)
            {
                throw new InvalidOperationException("Shell is invalid or null");
            }

            var rootFrame = shellWindow.Content as Frame;

            if (rootFrame == null)
            {
                // Create a Frame to act as the navigation context and navigate to the first page
                rootFrame = new Frame();

                // Place the frame in the current Window
                shellWindow.Content = rootFrame;
            }

            rootFrame.NavigationFailed += OnNavigationFailed;

            if (rootFrame.Content == null)
            {
                // When the navigation stack isn't restored navigate to the first page,
                // configuring the new page by passing required information as a navigation
                // parameter
                rootFrame.Content = this.RootPage;
            }

            Application.Current.MainWindow = shellWindow;

            shellWindow.Show();
        }

        #endregion

        #region Event Handlers

        private void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            throw new Exception("Failed to navigate to root page...");
        }

        #endregion
    }
}
