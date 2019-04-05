// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.UIManager.Events;
using System.Collections.Generic;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Provides <see cref="UIImplementation"/> to use in the 
    /// <see cref="UIManagerModule"/>.
    /// </summary>
    public class UIImplementationProvider
    {
        /// <summary>
        /// Creates the <see cref="UIImplementation"/> instance.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="viewManagers">The view managers.</param>
        /// <param name="eventDispatcher">The event dispatcher.</param>
        /// <returns>The <see cref="UIImplementation"/> instance.</returns>
        public UIImplementation Create(
            ReactContext reactContext,
            IReadOnlyList<IViewManager> viewManagers,
            EventDispatcher eventDispatcher)
        {
            return new UIImplementation(reactContext, viewManagers, eventDispatcher);
        }
    }
}
