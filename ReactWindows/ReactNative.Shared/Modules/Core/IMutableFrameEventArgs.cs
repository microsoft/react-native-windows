using System;

namespace ReactNative.Modules.Core
{
    interface IMutableFrameEventArgs
    {
        void Update(TimeSpan renderingTime);
    }
}
