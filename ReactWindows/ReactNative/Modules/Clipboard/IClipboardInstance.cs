// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.ApplicationModel.DataTransfer;

namespace ReactNative.Modules.Clipboard
{
    /// <summary>
    /// Gets and sets information from the clipboard object.
    /// </summary>
    public interface IClipboardInstance
    {
        /// <summary>
        /// Removes all data from the Clipboard.
        /// </summary>
        void Clear();

        /// <summary>
        /// Gets the current content that is stored in the clipboard object.
        /// </summary>
        /// <returns>Contains the content of the Clipboard.</returns>
        DataPackageView GetContent();

        /// <summary>
        /// Sets the current content that is stored in the clipboard object.
        /// </summary>
        /// <param name="package">Contains the content of the clipboard. If NULL, the clipboard is emptied.</param>
        void SetContent(DataPackage package);
    }
}
