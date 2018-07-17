// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative.Bridge
{
    /// <summary>
    /// Interface that represents a JavaScript callback function that can be
    /// passed to a native module as a method parameter.
    /// </summary>
    public interface ICallback
    {
        /// <summary>
        /// Invokes the callback.
        /// </summary>
        /// <param name="arguments">The callback arguments.</param>
        void Invoke(params object[] arguments);
    }
}
