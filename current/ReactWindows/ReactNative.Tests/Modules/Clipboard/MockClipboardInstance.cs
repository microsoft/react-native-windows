// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Modules.Clipboard;
using Windows.ApplicationModel.DataTransfer;

namespace ReactNative.Tests.Modules.Clipboard
{
    class MockClipboardInstance : IClipboardInstance
    {
        private DataPackage _package;

        public MockClipboardInstance()
        {
            _package = new DataPackage();
        }

        public void Clear()
        {
            _package.SetData(StandardDataFormats.Text, "");

        }

        public DataPackageView GetContent()
        {
            return _package.GetView();
        }

        public void SetContent(DataPackage package)
        {
            _package = package;
        }
    }
}
