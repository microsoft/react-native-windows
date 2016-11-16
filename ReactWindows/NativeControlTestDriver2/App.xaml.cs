using System.Windows;

namespace NativeControlTestDriver
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
        }

        /// <summary>
        /// Override method fired prior to the Startup event when the Run method of the Application object is called...
        /// </summary>
        /// <param name="e"></param>
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            var service = FirstFloor.XamlSpy.Services.XamlSpyService.Current;
            service.Connect("localhost", 4530, "12345");

            Application.Current.MainWindow = new MainWindow();
            Application.Current.MainWindow.Show();
        }
    }
}
