// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Devices.Input;

namespace ReactNative.Touch
{
    static class PointerDeviceTypeExtensions
    {
        public static string GetPointerDeviceTypeName(this PointerDeviceType pointerDeviceType)
        {
            switch (pointerDeviceType)
            {
                case PointerDeviceType.Touch:
                    return "touch";
                case PointerDeviceType.Pen:
                    return "pen";
                case PointerDeviceType.Mouse:
                    return "mouse";
                default:
                    return "unknown";
            }
        }
    }
}
