// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative
{
    /// <summary>
    /// An interface that holds the <see cref="ReactNativeHost"/>.
    /// </summary>
    public interface IReactApplication
    {
        /// <summary>
        /// The default <see cref="ReactNativeHost"/> for the app.
        /// </summary>
        ReactNativeHost Host { get; }
    }
}
