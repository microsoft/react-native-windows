using System;

namespace ReactNative.Modules.Core
{
    public interface IMutableFrameEventArgs
    {
        void Update(TimeSpan renderingTime);
    }
}
