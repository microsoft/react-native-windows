// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System.Collections.Generic;

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// JavaScript module for invoking timers by identifier.
    /// </summary>
    public sealed class JSTimers : JavaScriptModuleBase
    {
        /// <summary>
        /// Calls a batch of timers with the given identifiers.
        /// </summary>
        /// <param name="timerIds">The timer identifiers.</param>
        public void callTimers(IList<int> timerIds)
        {
            Invoke(timerIds);
        }
        
        /// <summary>
        /// Calls the idle callbacks with the current frame time.
        /// </summary>
        /// <param name="frameTime">The frame time.</param>
        public void callIdleCallbacks(long frameTime)
        {
            Invoke(frameTime);
        }
    }
}
