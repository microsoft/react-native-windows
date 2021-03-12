using NodeRpc;
using System;
using System.Collections.Generic;
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
            handler.BindOperation("DumpVisualTree", DumpVisualTree);

            var server = new Server(handler);
            var tsk = LoopServer(server);
        }

        JsonObject DumpVisualTree(JsonValue payload)
        {
            var payloadObj = payload.GetObject();
            var accessibilityId = payloadObj.GetNamedString("accessibilityId");

            var additionalProperties = new List<string>();
            if (payloadObj.ContainsKey("additionalProperties"))
            {
                foreach (JsonValue prop in payloadObj.GetNamedArray("additionalProperties"))
                {
                    additionalProperties.Add(prop.GetString());
                }
            }

            var rootDump = VisualTreeDumper.DumpTree(this, null, additionalProperties, new AttachedProperty[] { 
                new AttachedProperty() { Name = "Top", Property = Microsoft.ReactNative.ViewPanel.TopProperty },
                new AttachedProperty() { Name = "Left", Property = Microsoft.ReactNative.ViewPanel.LeftProperty }
            });
            var element = VisualTreeDumper.FindElementByAutomationId(JsonObject.Parse(rootDump), accessibilityId);

            return element;
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
