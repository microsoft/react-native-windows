// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// TODO

using System.Runtime.Serialization;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// TextDecorationLine values.
    /// </summary>
    public enum TextDecorationLine
    {
        /// <summary>
        /// Text has no line decoration.
        /// </summary>
        None,
        /// <summary>
        /// Text is Underlined.
        /// </summary>
        Underline,
        /// <summary>
        /// Text is Stroke out.
        /// </summary>
        LineThrough,
        /// <summary>
        /// Text is both Underlined and Stroke out.
        /// </summary>
        [EnumMember(Value = "underline line-through")]
        UnderlineLineThrough
    }
}
