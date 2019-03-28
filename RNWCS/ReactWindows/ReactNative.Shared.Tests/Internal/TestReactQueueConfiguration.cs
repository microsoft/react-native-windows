// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Bridge.Queue;
using System;

namespace ReactNative.Tests
{
    static class TestReactQueueConfiguration
    {
        public static IReactQueueConfiguration Create(Action<Exception> onError)
        {
            return new ReactQueueConfiguration(
                new ActionQueue(onError),
                new ActionQueue(onError),
                new ActionQueue(onError));
        }
    }
}
