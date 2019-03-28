// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.Views.TextInput
{
    /// <summary>
    /// Modes for the visibility state of the clear button in a ReactTextBox.
    /// </summary>
    internal enum ClearButtonModeType
    {
        /// <summary>
        /// The clear button will behave in default fashion.
        /// </summary>
        Default,
        /// <summary>
        /// The clear button will never appear.
        /// </summary>
        Never,
        /// <summary>
        /// The clear button will always appear.
        /// </summary>
        Always,
        /// <summary>
        /// The clear button will only appear while editing inside the textbox (while focused).
        /// </summary>
        WhileEditing,
        /// <summary>
        /// The clear button will only appear while NOT editing inside the textbox (while UNfocused).
        /// </summary>
        UnlessEditing
    }
}
