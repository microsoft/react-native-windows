using ReactNative;
using System;
using System.Windows;
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
            if (this.RootPage == null)
            {
                throw new InvalidOperationException("RootPage is null");
            }

            var shellViewModel = new ShellViewModel(this.RootPage);

            return new MainWindow(shellViewModel);
        }

        protected override void InitializeShell()
        {
            var shellWindow = this.Shell as MainWindow;

            if (shellWindow == null)
            {
                throw new InvalidOperationException("Shell is invalid or null");
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
