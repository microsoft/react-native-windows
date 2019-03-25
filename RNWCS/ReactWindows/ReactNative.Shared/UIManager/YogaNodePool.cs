// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Facebook.Yoga;
using ReactNative.Pooling;

namespace ReactNative.UIManager
{
    static class YogaNodePool
    {
        private static readonly object s_instanceLock = new object();
        private static ObjectPool<YogaNode> s_instance;

        public static ObjectPool<YogaNode> Instance
        {
            get
            {
                if (s_instance == null)
                {
                    lock (s_instanceLock)
                    {
                        if (s_instance == null)
                        {
                            s_instance = new ObjectPool<YogaNode>(CreateInstance, 1024);
                        }
                    }
                }

                return s_instance;
            }
        }

        private static YogaNode CreateInstance()
        {
            return new YogaNode(new YogaConfig
            {
                UseLegacyStretchBehaviour = true,
            });
        }
    }
}
