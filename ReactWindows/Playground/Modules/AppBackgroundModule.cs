using ReactNative.Bridge;

namespace Playground.Modules
{
    class AppBackgroundModule : JavaScriptModuleBase
    {
        public void doWork(string taskId, string deferralId)
        {
            Invoke(taskId, (object)deferralId);
        }
    }
}

