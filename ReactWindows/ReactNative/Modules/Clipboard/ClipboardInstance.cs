// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.ApplicationModel.DataTransfer;

namespace ReactNative.Modules.Clipboard
{
    class ClipboardInstance : IClipboardInstance
    {
        public void Clear()
        {
            Windows.ApplicationModel.DataTransfer.Clipboard.Clear();
        }

        public DataPackageView GetContent()
        {
            return Windows.ApplicationModel.DataTransfer.Clipboard.GetContent();
        }

        public void SetContent(DataPackage package)
        {
            Windows.ApplicationModel.DataTransfer.Clipboard.SetContent(package);
        }
    }
}
