using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed;

namespace SampleLibraryCS
{
    public sealed class CsStringsPackage : INativeModulePackage
    {
        public void CreateModuleProviders(ModuleProviderAdder addModuleProvider)
        {
            ReactModuleInfo.AddAttributedModules(addModuleProvider);
        }
    }
}
