using AutomationChannel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using TreeDumpLibrary;
using Windows.Data.Json;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace RNTesterApp
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


            var handler = new CommandHandler();
            handler.BindOperation("DumpVisualTree", DumpVisualTree);
            handler.BindOperation("ListErrors", ListErrors);

            var server = new Server(handler);
            var tsk = LoopServer(server);
        }

        JsonObject ListErrors(JsonValue payload)
        {
            JsonObject result = new JsonObject();
            var app = Application.Current as App;
            var jsonErrors = new JsonArray();
            var jsonWarnings = new JsonArray();
            foreach (var err in app.ConsoleErrors)
            {
                jsonErrors.Add(JsonValue.CreateStringValue(err));
            }
            foreach (var err in app.ConsoleWarnings)
            {
                jsonWarnings.Add(JsonValue.CreateStringValue(err));
            }
            result["errors"] = jsonErrors;
            result["warnings"] = jsonWarnings;
            app.ConsoleErrors.Clear();
            app.ConsoleWarnings.Clear();
            return result;
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

            var attachedProperties = new AttachedProperty[] {
                new AttachedProperty() { Name = "Top", Property = Microsoft.ReactNative.ViewPanel.TopProperty },
                new AttachedProperty() { Name = "Left", Property = Microsoft.ReactNative.ViewPanel.LeftProperty },
                new AttachedProperty() { Name = "Tooltip", Property = Windows.UI.Xaml.Controls.ToolTipService.ToolTipProperty },
                new AttachedProperty() { Name = "AutomationLevel", Property = Windows.UI.Xaml.Automation.AutomationProperties.LevelProperty },
                new AttachedProperty() { Name = "AutomationSizeOfSet", Property = Windows.UI.Xaml.Automation.AutomationProperties.SizeOfSetProperty },
                new AttachedProperty() { Name = "AutomationPositionInSet", Property = Windows.UI.Xaml.Automation.AutomationProperties.PositionInSetProperty },
                new AttachedProperty() { Name = "AccessibilityRole", Property = Microsoft.ReactNative.DynamicAutomationProperties.AccessibilityRoleProperty },
                new AttachedProperty() { Name = "AccessibilityStateSelected", Property = Microsoft.ReactNative.DynamicAutomationProperties.AccessibilityStateSelectedProperty },
                new AttachedProperty() { Name = "AccessibilityStateDisabled", Property = Microsoft.ReactNative.DynamicAutomationProperties.AccessibilityStateDisabledProperty },
                new AttachedProperty() { Name = "AccessibilityStateChecked", Property = Microsoft.ReactNative.DynamicAutomationProperties.AccessibilityStateCheckedProperty },
                new AttachedProperty() { Name = "AccessibilityStateBusy", Property = Microsoft.ReactNative.DynamicAutomationProperties.AccessibilityStateBusyProperty },
                new AttachedProperty() { Name = "AccessibilityStateExpanded", Property = Microsoft.ReactNative.DynamicAutomationProperties.AccessibilityStateExpandedProperty },
            };
            var rootDump = VisualTreeDumper.DumpTree(this, null, additionalProperties, attachedProperties);
            rootDump = rootDump.Replace(" False", " false").Replace(" True", " true"); // Temporary workaround until we can fix upstream to produce correct JSON boolean values
            var element = VisualTreeDumper.FindElementByAutomationId(JsonObject.Parse(rootDump), accessibilityId);
            if (element != null)
                return element;

            if (accessibilityId.StartsWith("*"))
            {
                accessibilityId = accessibilityId.Substring(1);

                foreach (var popup in VisualTreeHelper.GetOpenPopups(Window.Current))
                {
                    // XAML's VisualTreeHelper returns 0 for GetChildrenCount of a popup root, so use the popup's child
                    var popupChildDump = VisualTreeDumper.DumpTree(popup.Child, null, additionalProperties, attachedProperties);

                    var json = JsonObject.Parse(popupChildDump);
                    if (accessibilityId == "")
                    {
                        element = json;
                    }
                    else
                    {
                        element = VisualTreeDumper.FindElementByAutomationId(json, accessibilityId);
                    }
                    if (element != null) break;
                }
            }
            return element;
        }

        async Task LoopServer(Server server)
        {
            while (true)
            {
                try
                {
                    await server.ProcessAllClientRequests(8603, TimeSpan.FromMilliseconds(50));
                }
                catch (Exception ex)
                {
                }
            }
        }

    }
}
