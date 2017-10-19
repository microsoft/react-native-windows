using ReactNative.Bridge;

namespace Playground.Modules
{
    class BackgroundRegistrationModule : JavaScriptModuleBase
    {
        public void unregister(string id)
        {
            Invoke((object)id);
        }
    }
}
