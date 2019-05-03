// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Windows;

namespace ReactNative.Modules.Clipboard
{
    class ClipboardInstance : IClipboardInstance
    {
        public void Clear()
        {
            System.Windows.Clipboard.Clear();
        }

        public IDataObject GetContent()
        {
            return System.Windows.Clipboard.GetDataObject();
        }

        public void SetContent(IDataObject package)
        {
            System.Windows.Clipboard.SetDataObject(package);
        }

        public bool ContainsText()
        {
            return System.Windows.Clipboard.ContainsText();
        }

        public string GetText()
        {
            return System.Windows.Clipboard.GetText();
        }

        public void SetText(string str)
        {
            System.Windows.Clipboard.SetText(str);
        }
    }
}
