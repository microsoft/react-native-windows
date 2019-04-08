// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.Bridge
{
    /// <summary>
    /// An interface for native modules with codegen wrappers.
    /// </summary>
    /// <remarks>
    /// Primarily used to implement <see cref="ReactContext.GetNativeModule{T}"/>.
    /// </remarks>
    public interface INativeModuleWrapper
    {
        /// <summary>
        /// The native module instance.
        /// </summary>
        INativeModule Module { get; }
    }
}
