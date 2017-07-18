using Newtonsoft.Json.Linq;

namespace ReactNative.Bridge
{
    class Callback : ICallback
    {
        private static readonly JArray s_empty = new JArray();

        private readonly int _id;
        private readonly IReactInstance _instance;

        public Callback(int id, IReactInstance instance)
        {
            _id = id;
            _instance = instance;
        }

        public void Invoke(params object[] arguments)
        {
            _instance.InvokeCallback(_id, arguments != null ? JArray.FromObject(arguments) : s_empty);
        }
    }
}
