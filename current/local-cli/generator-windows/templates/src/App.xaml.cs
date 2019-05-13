using ReactNative;

namespace <%=ns%>
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    sealed partial class App : ReactApplication
    {
        private readonly ReactNativeHost _host = new MainReactNativeHost();

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// The React Native host.
        /// </summary>
        public override ReactNativeHost Host => _host;
    }
}
