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
using System.Collections.Generic;

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
                var bounds = ApplicationView.GetForCurrentView().VisibleBounds;
                if (bounds.Width != 800 || bounds.Height != 600)
                {
                    // Dump disabled when window size is not 800x600!
                    UpdateResult(false /*matchDump*/, "Window has been resized, dump comparison is only valid at default launch size: 800x600!, current size:" + bounds.ToString());
                }
                else
                {
                    // delay dumping tree by 100ms for layout to stablize
                    if (m_timer != null)
                    {
                        m_timer.Stop();
                        m_timer.Start();
                    }
                }
            };

            m_textBlock.PointerPressed += (soruce, e) =>
            {
                if (!m_dumpMatchExpected)
                {
                    if (m_timer != null)
                    {
                        m_timer.Stop();
                    }
                    m_errStringShowing = true;
                    m_textBlock.Text = m_errString;
                    m_textBlock.IsTextSelectionEnabled = true;
                }
            };

            return m_textBlock;
        }

        public void UpdateProperties(FrameworkElement view, IReadOnlyDictionary<string, object> propertyMap)
        {
            foreach (KeyValuePair<string, object> kvp in propertyMap)
            {
                if (kvp.Key == "dumpID")
                {
                    SetDumpID((TextBlock)view, (string)kvp.Value);
                }
                else if (kvp.Key == "uiaID")
                {
                    SetUIAID((TextBlock)view, (string)kvp.Value);
                }
            }
        }

        IReadOnlyDictionary<string, ViewManagerPropertyType> IViewManagerWithNativeProperties.NativeProps
        {
            get
            {
                return new Dictionary<string, ViewManagerPropertyType>
                {
                    { "dumpID", ViewManagerPropertyType.String },
                    { "uiaID", ViewManagerPropertyType.String }
                };
            }
        }

        public void SetDumpID(TextBlock view, string value)
        {
            m_dumpID = value;
            m_dumpMatchExpected = false;
            m_dumpExpectedText = null;
            m_errString = "";
            m_errStringShowing = false;
            if (m_textBlock != null)
            {
                m_textBlock.IsTextSelectionEnabled = false;
            }
            m_timer = new DispatcherTimer();
            m_timer.Tick += dispatcherTimer_Tick;
            m_timer.Interval = new TimeSpan(0, 0, 0, 0, 100);
        }

        public void SetUIAID(TextBlock view, string value)
        {
            m_uiaID = value;
        }

        private async void dispatcherTimer_Tick(object sender, object e)
        {
            m_timer.Stop();
            if (VisualTreeHelper.GetParent(m_textBlock) != null)
            {
                await MatchTreeDumpFromLayoutUpdateAsync();
            }
        }
        private DependencyObject FindChildWithMatchingUIAID(DependencyObject element)
        {
            string automationId = (string)element.GetValue(Windows.UI.Xaml.Automation.AutomationProperties.AutomationIdProperty);
            if (automationId == m_uiaID)
            {
                return element;
            }
            int childrenCount = VisualTreeHelper.GetChildrenCount(element);
            for (int i = 0; i < childrenCount; i++)
            {
                var result = FindChildWithMatchingUIAID(VisualTreeHelper.GetChild(element, i));
                if (result != null)
                {
                    return result;
                }
            }

            return null;
        }

        private async Task MatchTreeDumpFromLayoutUpdateAsync()
        {
            // First find root
            DependencyObject current = m_textBlock;
            DependencyObject parent = VisualTreeHelper.GetParent(current);
            while (parent != null)
            {
                current = parent;
                parent = VisualTreeHelper.GetParent(current);
            }

            DependencyObject dumpRoot = current;
            // if UIAID is passed in from test, find the matching child as the root to dump
            if (m_uiaID != null)
            {
                var matchingNode = FindChildWithMatchingUIAID(current);
                if (matchingNode != null)
                {
                    dumpRoot = matchingNode;
                }
            }

            String dumpText = VisualTreeDumper.DumpTree(dumpRoot, m_textBlock /* exclude */);
            if (dumpText != m_dumpExpectedText)
            {
                await MatchDump(dumpText);
            }
        }

        private async Task MatchDump(string dumpText)
        {
            if (m_dumpExpectedText == null)
            {
                try
                {
                    var file = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync(@"Assets\TreeDump\" + m_dumpID + ".txt");
                    m_dumpExpectedText = await Windows.Storage.FileIO.ReadTextAsync(file);

                    StorageFolder storageFolder = ApplicationData.Current.LocalFolder;
                    string copyFileName = "TreeDump\\" + m_dumpID + ".txt";
                    var copyDumpFile = await storageFolder.CreateFileAsync(copyFileName, CreationCollisionOption.ReplaceExisting);
                    await Windows.Storage.FileIO.WriteTextAsync(copyDumpFile, m_dumpExpectedText);
                }
                catch (IOException)
                {
                    UpdateResult(false /*matchDump*/, "Tree dump master file not found in testapp package!");
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
                    UpdateResult(false /*matchDump*/, "Tree dump file does not match master! See output at " + outFile.Path);
                }
                catch (IOException)
                {
                    UpdateResult(false /*matchDump*/, "Can't write dump output file:" + fileName);
                }
            }
            else
            {
                UpdateResult(true /*matchDump*/, "");
            }
        }

        private async void UpdateResult(bool matchDump, string helpText)
        {
            if (matchDump)
            {
                UpdateTextBlockText("TreeDump:Passed");
            }
            else
            {
                UpdateTextBlockText("TreeDump:Failed, click to see more!");
                m_errString += "\r\n" + helpText;

                StorageFolder storageFolder = ApplicationData.Current.LocalFolder;
                string fileNameError = "TreeDump\\" + m_dumpID + ".err";
                try
                {
                    StorageFile errFile = await storageFolder.CreateFileAsync(fileNameError, CreationCollisionOption.GenerateUniqueName);
                    await Windows.Storage.FileIO.WriteTextAsync(errFile, m_errString);
                }
                catch (Exception e)
                {
                    UpdateTextBlockText("Creat err file failed: " + e.ToString());
                }
            }

            m_dumpMatchExpected = matchDump;
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
        private string m_uiaID = null;

        private DispatcherTimer m_timer = null;

    }
}
