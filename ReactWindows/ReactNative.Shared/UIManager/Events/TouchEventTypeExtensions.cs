// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;

namespace ReactNative.UIManager.Events
{
    static class TouchEventTypeExtensions
    {
        public static string GetJavaScriptEventName(this TouchEventType eventType)
        {
            switch (eventType)
            {
                case TouchEventType.Start:
                    return "topTouchStart";
                case TouchEventType.End:
                    return "topTouchEnd";
                case TouchEventType.Move:
                    return "topTouchMove";
                case TouchEventType.Cancel:
                    return "topTouchCancel";
                case TouchEventType.Entered:
                    return "topMouseOver";
                case TouchEventType.Exited:
                    return "topMouseOut";
                case TouchEventType.PointerMove:
                    return "topMouseMoveCustom"; // Using a non-clashing name until this one propagates: https://github.com/facebook/react/commit/e96dc140599363029bd05565d58bcd4a432db370
                default:
                    throw new NotSupportedException("Unsupported touch event type.");
            }
        }
    }
}
