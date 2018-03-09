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
                            s_instance = new ObjectPool<YogaNode>(() => new YogaNode(), 1024);
                        }
                    }
                }

                return s_instance;
            }
        }
    }
}
