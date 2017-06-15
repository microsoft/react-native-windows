using System;

namespace ReactNative.Bridge
{
    public interface INativeModuleWrapper
    {
        Type ModuleType { get; }

        INativeModule Module { get; }
    }
}
