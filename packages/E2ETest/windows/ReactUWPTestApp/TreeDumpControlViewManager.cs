// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Windows.ApplicationModel.ConversationalAgent;
using Windows.ApplicationModel.DataTransfer;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace TreeDumpLibrary
{
    internal class TreeDumpControlViewManager : IViewManager, IViewManagerWithNativeProperties
    {
        public string Name => "TreeDumpControl";

        public FrameworkElement CreateView()
        {
            m_textBlock = new TextBlock();
            m_textBlock.TextWrapping = TextWrapping.Wrap;
            m_textBlock.IsTextSelectionEnabled = false;
            m_textBlock.LayoutUpdated += async (source, e) =>
            {
                await KnownFolders.DocumentsLibrary.CreateFolderAsync("TreeDump", CreationCollisionOption.OpenIfExists);
                ApplicationView.GetForCurrentView().TryResizeView(new Size(800, 600));
                var bounds = ApplicationView.GetForCurrentView().VisibleBounds;
                if (bounds.Width != 800 || bounds.Height != 600)
                {
                    // Dump disabled when window size is not 800x600!
                    UpdateResult(false /*matchDump*/ , $"Window has been resized, dump comparison is only valid at default launch size: 800x600!, current size: {bounds.Width}x{bounds.Height}");
                }
                else
                {
                    // delay dumping tree by 100ms for layout to stabilize
                    if (m_timer != null)
                    {
                        m_timer.Stop();
                        m_timer.Start();
                    }
                }
            };

            m_textBlock.PointerPressed += (source, e) =>
            {
                if (!m_dumpMatchExpected)
                {
                    if (m_timer != null)
                    {
                        m_timer.Stop();
                    }
                    m_errStringShowing = true;
                    errors.Apply(m_textBlock);
                    m_textBlock.IsTextSelectionEnabled = true;
                }
            };

            return m_textBlock;
        }

        public void UpdateProperties(FrameworkElement view, IJSValueReader propertyMapReader)
        {
            var propertyMap = JSValue.ReadObjectFrom(propertyMapReader);
            foreach (KeyValuePair<string, JSValue> kvp in propertyMap)
            {
                if (kvp.Key == "dumpID")
                {
                    SetDumpID((TextBlock)view, kvp.Value.AsString());
                }
                else if (kvp.Key == "uiaID")
                {
                    SetUIAID((TextBlock)view, kvp.Value.AsString());
                }
                else if (kvp.Key == "additionalProperties")
                {
                    SetAdditionalProperties(kvp.Value.AsArray());
                }
            }
        }

        IReadOnlyDictionary<string, ViewManagerPropertyType> IViewManagerWithNativeProperties.NativeProps => new Dictionary<string, ViewManagerPropertyType>
        { { "dumpID", ViewManagerPropertyType.String },
            { "uiaID", ViewManagerPropertyType.String },
            { "additionalProperties", ViewManagerPropertyType.Array }
        };

        private static string GetOutputFile(string dumpID)
        {
            return "TreeDump\\" + dumpID + ".json";
        }

        private static string GetMasterFile(string dumpID)
        {
            return "TreeDump\\masters\\" + dumpID + ".json";
        }

        public async static Task<bool> DoesTreeDumpMatchForRNTester(DependencyObject root)
        {
            string json = VisualTreeDumper.DumpTree(root, null, new string[] { }, DumpTreeMode.Json);
            try
            {
                var obj = JsonValue.Parse(json).GetObject();
                var element = VisualTreeDumper.FindElementByAutomationId(obj, "PageHeader");
                if (element == null)
                {
                    return false;
                }
                var value = element.GetNamedString("Text");
                var pageName = new Regex(@"[<|>]").Replace(value, "");
                var match = await MatchDump(json, GetMasterFile(pageName), GetOutputFile(pageName));
                return match;
            }
            catch
            {
                Debug.WriteLine("JSON ERROR:\n" + json);
                throw;
            }
        }

        public void SetDumpID(TextBlock view, string value)
        {
            m_dumpID = value;
            m_dumpMatchExpected = false;
            m_dumpExpectedText = null;
            m_errString = "";
            errors = new TreeDumpErrors();
            m_errStringShowing = false;
            if (m_textBlock != null)
            {
                m_textBlock.IsTextSelectionEnabled = false;
                UpdateTextBlockText("");
            }
            m_timer = new DispatcherTimer();
            m_timer.Tick += dispatcherTimer_Tick;
            m_timer.Interval = new TimeSpan(0, 0, 0, 0, 200);
        }

        public void SetUIAID(TextBlock view, string value)
        {
            m_uiaID = value;
        }

        public void SetAdditionalProperties(IReadOnlyList<JSValue> additionalProperties)
        {
            foreach (var property in additionalProperties)
            {
                m_additionalProperties.Add(property.AsString());
            }
        }

        public static async Task<bool> MatchDump(string outputJson, string masterFileRelativePath, string outputFileRelativePath)
        {
            Debug.WriteLine($"master file = {Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\Assets\\{masterFileRelativePath}");
            StorageFolder storageFolder = KnownFolders.DocumentsLibrary;
            Debug.WriteLine($"output file = {storageFolder.Path + "\\" + outputFileRelativePath}");

            StorageFile outFile = await storageFolder.CreateFileAsync(outputFileRelativePath, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(outFile, outputJson);

            StorageFile masterFile = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync($@"Assets\{masterFileRelativePath}");

            string masterJson = await FileIO.ReadTextAsync(masterFile);

            if (!TreeDumpHelper.DumpsAreEqual(masterJson, outputJson))
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        internal static async Task<bool> MatchTreeDumpFromLayoutUpdateAsync(string dumpID, string uiaId, TextBlock textBlock, IList<string> additionalProperties, DumpTreeMode mode, string dumpExpectedText)
        {
            // First find root
            DependencyObject current = textBlock;
            DependencyObject parent = VisualTreeHelper.GetParent(current);
            while (parent != null)
            {
                current = parent;
                parent = VisualTreeHelper.GetParent(current);
            }

            DependencyObject dumpRoot = current;
            // if UIAID is passed in from test, find the matching child as the root to dump
            if (uiaId != null)
            {
                var matchingNode = TreeDumpHelper.FindChildWithMatchingUIAID(current, uiaId);
                if (matchingNode != null)
                {
                    dumpRoot = matchingNode;
                }
            }

            string dumpText = VisualTreeDumper.DumpTree(dumpRoot, textBlock, additionalProperties, mode);

            if (dumpText != dumpExpectedText)
            {
                return await MatchDump(dumpText, GetMasterFile(dumpID), GetOutputFile(dumpID));
            }
            return true;
        }

        private async void dispatcherTimer_Tick(object sender, object e)
        {
            m_timer.Stop();
            if (VisualTreeHelper.GetParent(m_textBlock) != null)
            {
                var matchSuccessful = await MatchTreeDumpFromLayoutUpdateAsync(m_dumpID, m_uiaID, m_textBlock, m_additionalProperties, DumpTreeMode.Json, m_dumpExpectedText);
                if (!matchSuccessful)
                {
                    StorageFolder storageFolder = KnownFolders.DocumentsLibrary;
                    StorageFile outFile = await storageFolder.GetFileAsync(GetOutputFile(m_dumpID));
                    StorageFile masterFile = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync($@"Assets\{GetMasterFile(m_dumpID)}");

                    UpdateResult(false /*matchDump*/ ,
                        $"Tree dump file does not match master at {masterFile.Path} - See output at {outFile.Path}",
                        GetInlines(masterFile, outFile, m_textBlock));
                }
                else
                {
                    UpdateResult(true /*matchDump*/ , "");
                }

            }
        }

        private static IList<Inline> GetInlines(StorageFile masterFile, StorageFile outFile, UIElement anchor)
        {
            Hyperlink masterLink = new Hyperlink();
            masterLink.Click += async (_1, _2) => { await Windows.System.Launcher.LaunchFileAsync(masterFile); };
            masterLink.Inlines.Add(new Run() { Text = "master" });
            Hyperlink outLink = new Hyperlink();
            outLink.Click += async (_1, _2) => { await Windows.System.Launcher.LaunchFileAsync(outFile); };
            outLink.Inlines.Add(new Run() { Text = "output" });
            List<Inline> inlines = new List<Inline>()
            {
                new Run () { Text = "Tree dump " },
                outLink,
                new Run () { Text = " does not match " },
                masterLink
            };

            #region Diff support - Replace with LaunchUriAsync when we find the VSCode protocol handler Uri for diffing
            string code_cmd = Environment.ExpandEnvironmentVariables(@"%UserProfile%\AppData\Local\Programs\Microsoft VS Code\bin\code.cmd");
            Hyperlink diffLink = new Hyperlink();
            diffLink.Click += (_1, _2) =>
            {
                string commandLine = $"code.cmd --diff \"{masterFile.Path}\" \"{outFile.Path}\"";
                DataPackage dataPackage = new DataPackage();
                dataPackage.SetText(commandLine);
                Clipboard.SetContent(dataPackage);
                ToolTip toolTip = new ToolTip() { Content = "Copied to clipboard" };
                ToolTipService.SetToolTip(anchor, toolTip);
                toolTip.Opened += (_3, _4) =>
                {
                    var timer = Windows.System.DispatcherQueue.GetForCurrentThread().CreateTimer();
                    timer.IsRepeating = false;
                    timer.Interval = TimeSpan.FromSeconds(1);
                    timer.Tick += (_5, _6) =>
                    {
                        toolTip.IsOpen = false;
                        ToolTipService.SetToolTip(anchor, null);
                    };
                    timer.Start();
                };
                toolTip.IsOpen = true;
            };
            diffLink.Inlines.Add(new Run() { Text = "copy diff command to clipboard" });
            inlines.Add(new Run() { Text = " - " });
            inlines.Add(diffLink);
            #endregion
            return inlines;
        }

        private async void UpdateResult(bool matchDump, string helpText, IList<Inline> inlines = null)
        {
            if (matchDump)
            {
                UpdateTextBlockText("TreeDump:Passed");
            }
            else
            {
                UpdateTextBlockText("TreeDump:Failed, click to see more!");
                m_errString += "\r\n" + helpText;
                errors.Inlines.Clear();
                if (inlines != null)
                {
                    errors.Inlines.AddRange(inlines);
                }
                else
                {
                    errors.Inlines.Add(new Run() { Text = helpText });
                }
                errors.Inlines.Add(new LineBreak());
                await WriteErrorFile();
            }

            m_dumpMatchExpected = matchDump;
        }

        private async Task WriteErrorFile()
        {
            StorageFolder storageFolder = KnownFolders.DocumentsLibrary;
            string fileNameError = "TreeDump\\" + m_dumpID + ".err";
            try
            {
                StorageFile errFile = await storageFolder.CreateFileAsync(fileNameError, CreationCollisionOption.GenerateUniqueName);
                await FileIO.WriteTextAsync(errFile, m_errString);
            }
            catch (Exception e)
            {
                UpdateTextBlockText("Create err file failed: " + e.ToString());
            }
        }

        private void UpdateTextBlockText(string text)
        {
            if (!m_errStringShowing && m_textBlock.Text != text)
            {
                m_textBlock.Text = text;
            }
        }
        private TextBlock m_textBlock = null;
        private string m_dumpID = "UnknownTest";
        private string m_dumpExpectedText;
        private bool m_dumpMatchExpected = false;
        private bool m_errStringShowing = false;
        private string m_errString = "";
        private TreeDumpErrors errors = new TreeDumpErrors();
        private string m_uiaID = null;
        private readonly List<string> m_additionalProperties = new List<string>();

        private DispatcherTimer m_timer = null;
    }

    internal class TreeDumpErrors
    {
        readonly List<Inline> inlines = new List<Inline>();

        public List<Inline> Inlines => inlines;

        public void Apply(TextBlock textBlock)
        {
            textBlock.Text = "";
            textBlock.Inlines.Clear();
            foreach (var inline in Inlines)
            {
                textBlock.Inlines.Add(inline);
            }
            textBlock.Width = 800;
        }
    }
}
