using ReactNative;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Navigation;

namespace Playground.Net46
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        //private Window _shellWindow;
        private ReactPage _reactPage;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            _reactPage = new AppReactPage();

            _reactPage.OnResume(() =>
            {
                //OnBackPressed
                Application.Current.Shutdown(0);
            });

            //TODO: UWP's OnLaunched prvoides arguments as a string, whereas WPF's OnStartup provides them as a string[]; Need to figure out what format the ReactPage.OnCreate wants...
            _reactPage.OnCreate(null);

            // Create a Frame to act as the navigation context and navigate to the first page
            var rootFrame = new Frame
            {
                Height = 300,
                Width = 400,
                Content = _reactPage
            };

            rootFrame.NavigationFailed += OnNavigationFailed;

            //Create a Window to hold the frame
            var shellWindow = new Window
            {
                Background = new SolidColorBrush(Colors.BlueViolet),
                Height = 300,
                Width = 400,
                ShowActivated = true,
                ShowInTaskbar = true,
                WindowStartupLocation = WindowStartupLocation.CenterScreen,
                Content = rootFrame
            };

            Application.Current.MainWindow = shellWindow;

            shellWindow.Show();
        }

        /// <summary>
        /// Invoked when Navigation to a certain page fails
        /// </summary>
        /// <param name="sender">The Frame which failed navigation</param>
        /// <param name="e">Details about the navigation failure</param>
        private void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            throw new Exception("Failed to load Page");
        }


    }
}
