// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using static System.FormattableString;

namespace ReactNative.Views.Scroll
{
    static class ScrollEventTypeExtensions
    {
        public static string GetJavaScriptEventName(this ScrollEventType type)
        {
            switch (type)
            {
                case ScrollEventType.BeginDrag:
                    return "topScrollBeginDrag";
                case ScrollEventType.EndDrag:
                    return "topScrollEndDrag";
                case ScrollEventType.Scroll:
                    return "topScroll";
                default:
                    throw new ArgumentOutOfRangeException(nameof(type), Invariant($"Unknown scroll event type '{type}'."));
            }
        }
    }
}
