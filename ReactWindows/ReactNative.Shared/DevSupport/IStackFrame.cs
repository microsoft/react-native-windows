// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.DevSupport
{
    /// <summary>
    /// A data container for call stack information.
    /// </summary>
    public interface IStackFrame
    {
        /// <summary>
        /// The method name.
        /// </summary>
        string Method { get; }

        /// <summary>
        /// The line number.
        /// </summary>
        int Line { get; }

        /// <summary>
        /// The column number.
        /// </summary>
        int Column { get; }

        /// <summary>
        /// The file name.
        /// </summary>
        string FileName { get; }

        /// <summary>
        /// The formatted source information.
        /// </summary>
        string SourceInfo { get; }
    }
}
