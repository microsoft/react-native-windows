// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative.UIManager.Events
{
    /// <summary>
    /// Touch event types that the JavaScript module <see cref="RCTEventEmitter"/>
    /// understands.
    /// </summary>
    public enum TouchEventType
    {
        /// <summary>
        /// Touch start event type.
        /// </summary>
        Start,

        /// <summary>
        /// Touch end event type.
        /// </summary>
        End,

        /// <summary>
        /// Touch move event type.
        /// </summary>
        Move,

        /// <summary>
        /// Touch cancel event type.
        /// </summary>
        Cancel,

        /// <summary>
        /// Pointer entered event type.
        /// </summary>
        Entered,

        /// <summary>
        /// Pointer exited event type.
        /// </summary>
        Exited,

        /// <summary>
        /// Pointer move event type.
        /// </summary>
        PointerMove
    }
}
