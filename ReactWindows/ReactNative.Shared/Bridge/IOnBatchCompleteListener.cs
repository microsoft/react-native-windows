// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative.Bridge
{
    /// <summary>
    /// Interface that will be notified when a batch of JavaScript to native
    /// calls has finished.
    /// </summary>
    public interface IOnBatchCompleteListener
    {
        /// <summary>
        /// Invoked when a batch of JavaScript to native calls has finished.
        /// </summary>
        void OnBatchComplete();
    }
}
