using System.Windows;

namespace Playground.Net46
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        private readonly PlaygroundBootstrapper _bootstrapper = new PlaygroundBootstrapper();

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            this._bootstrapper.Run("");
        }
    }
}
