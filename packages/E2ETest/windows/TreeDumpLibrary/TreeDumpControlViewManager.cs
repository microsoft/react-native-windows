// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Diagnostics;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls;

using Microsoft.ReactNative.Managed;
using Microsoft.ReactNative.Bridge;
using Windows.UI.Xaml;
using Windows.UI.ViewManagement;
using System.Threading.Tasks;
using System;
using System.IO;
using Windows.Storage;

namespace TreeDumpLibrary
{
    internal class TreeDumpControlViewManager : AttributedViewManager<TextBlock>
    {
        enum TreeDumpMatchResult { Disabled, Passed, Failed};
        public override string Name => "TreeDumpControl";
        public TreeDumpControlViewManager(IReactContext reactContext) : base(reactContext) { }

        public override FrameworkElement CreateView()
        {
            m_textBlock = new TextBlock();
            m_textBlock.TextWrapping = TextWrapping.Wrap;
            m_textBlock.LayoutUpdated += async (source, e) =>
            {
                var bounds = ApplicationView.GetForCurrentView().VisibleBounds;
                if (bounds.Width != 1024 || bounds.Height != 768)
                {
                    // Dump disabled when window size is not 1024x768!
                    UpdateResult(TreeDumpMatchResult.Disabled, "Window has been resized, dump comparison is only valid at default launch size: 1024x768!");
                }
                else
                {
                    await MatchTreeDumpFromLayoutUpdateAsync();
                }
            };

            m_textBlock.PointerPressed += (soruce, e) =>
            {
                if (m_matchResult != TreeDumpMatchResult.Passed)
                {
                    m_textBlock.Text = m_matchHelpString;
                    m_textBlock.IsTextSelectionEnabled = true;
                }
            };

            return m_textBlock;
        }

        [ViewManagerProperty("dumpID")]
        public void SetDumpID(TextBlock view, string value)
        {
            m_dumpID = value;
        }

        private async Task MatchTreeDumpFromLayoutUpdateAsync()
        {
            DependencyObject parent = VisualTreeHelper.GetParent(m_textBlock);
            while (parent != null)
            {
                var name = ((FrameworkElement)parent).Name;
                if (name == "RNRootView")
                {
                    String dumpText = VisualTreeDumper.DumpToXML(parent, m_textBlock /* exclude */);
                    if (dumpText != m_dumpExpectedText)
                    {
                        await MatchDump(dumpText);
                    }
                }
                parent = VisualTreeHelper.GetParent(parent);
            }
        }

        private async Task MatchDump(string dumpText)
        {
            if (m_dumpExpectedText == null)
            {
                try
                {
                    var file = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync(@"Assets\TreeDump\" + m_dumpID + ".treedump");
                    m_dumpExpectedText = await Windows.Storage.FileIO.ReadTextAsync(file);
                }
                catch (IOException)
                {
                    UpdateResult(TreeDumpMatchResult.Failed, "Tree dump master file not found in testapp package!");
                }
            }

            if (m_dumpExpectedText != dumpText)
            {
                StorageFolder storageFolder = ApplicationData.Current.LocalFolder;
                string fileName = "TreeDump\\" + m_dumpID + ".out";
                try
                {
                    StorageFile outFile = await storageFolder.CreateFileAsync(fileName, CreationCollisionOption.ReplaceExisting);
                    await Windows.Storage.FileIO.WriteTextAsync(outFile, dumpText);
                    UpdateResult(TreeDumpMatchResult.Failed, "Tree dump file does not match master! See output at " + outFile.Path);
                }
                catch (IOException)
                {
                    UpdateResult(TreeDumpMatchResult.Failed, "Can't write dump output file:" + fileName);
                }
            }
            else
            {
                UpdateResult(TreeDumpMatchResult.Passed, "");
            }
        }

        private void UpdateResult(TreeDumpMatchResult result, string helpText)
        {
            if (m_matchResult != result)
            {
                switch (result)
                {
                    case TreeDumpMatchResult.Disabled:
                        m_textBlock.Text = "TreeDump:Disabled, click to see more!";
                        break;
                    case TreeDumpMatchResult.Failed:
                        m_textBlock.Text = "TreeDump:Failed, click to see more!";
                        break;
                    case TreeDumpMatchResult.Passed:
                        m_textBlock.Text = "TreeDump:Passed";
                        break;
                }
                m_matchResult = result;
                m_matchHelpString = helpText;
            }
        }

        private TextBlock m_textBlock = null;
        private string m_dumpID = "UnknownTest";
        private string m_dumpExpectedText;
        private TreeDumpMatchResult m_matchResult = TreeDumpMatchResult.Disabled;
        private string m_matchHelpString = "No help text";
    }
}
