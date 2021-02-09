using NodeRpc;
using System;
using System.Threading.Tasks;
using TreeDumpLibrary;
using Windows.Data.Json;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace ReactUWPTestApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            var app = Application.Current as App;
            myRootView.ReactNativeHost = app.Host;
            myRootView.ComponentName = "RNTesterApp";


            var handler = new Handler();
            handler.BindOperation("DumpVisualTree", (payload) =>
            {
                var rootDump = VisualTreeDumper.DumpTree(this, null, new string[] { }, DumpTreeMode.Json);

                var accessibilityId = payload.GetObject().GetNamedString("accessibilityId");
                var element = VisualTreeDumper.FindElementByAutomationId(JsonObject.Parse(rootDump), accessibilityId);

                return element;
            });

            var server = new Server(handler);
            var tsk = LoopServer(server);
        }

        async Task LoopServer(Server server)
        {
            while (true)
            {
                await server.ProcessAllClientRequests(8603, TimeSpan.FromMilliseconds(50));
            }
        }

    }
}
