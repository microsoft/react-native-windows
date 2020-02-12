// Copyright (c) Microsoft Corporation.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative.Bridge
{
    /// <summary>
    /// An interface for JavaScript modules.
    /// </summary>
    public interface IJavaScriptModule
    {
        /// <summary>
        /// The invocation handler.
        /// </summary>
        IInvocationHandler InvocationHandler { set; }
    }
}
