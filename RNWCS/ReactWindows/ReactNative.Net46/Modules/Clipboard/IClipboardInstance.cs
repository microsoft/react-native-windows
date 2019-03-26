// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Windows;

namespace ReactNative.Modules.Clipboard
{
    /// <summary>
    /// Gets and sets information from the clipboard object.
    /// </summary>
    public interface IClipboardInstance
    {
        /// <summary>
        /// Removes all data from the clipboard.
        /// </summary>
        void Clear();

        /// <summary>
        /// Gets the current content that is stored in the clipboard object.
        /// </summary>
        /// <returns>Contains the content of the clipboard.</returns>
        IDataObject GetContent();

        /// <summary>
        /// Sets the current content that is stored in the clipboard object.
        /// </summary>
        /// <param name="package">Contains the content of the clipboard. If <code>NULL</code>, the clipboard is emptied.</param>
        void SetContent(IDataObject package);

        /// <summary>
        /// Gets if the clipboard contains text.
        /// </summary>
        /// <returns></returns>
        bool ContainsText();

        /// <summary>
        /// Gets text from the clipboard
        /// </summary>
        /// <returns></returns>
        string GetText();

        /// <summary>
        /// Sets the clipboard text to a designated string
        /// </summary>
        /// <param name="str">String contents for the clipboard</param>
        void SetText(string str);
    }
}
