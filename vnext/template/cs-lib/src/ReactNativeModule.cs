using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;

namespace {{ namespace }}
{
    [ReactModule]
    internal sealed class ReactNativeModule
    {
        [ReactConstant]
        public bool const1 => true;

        [ReactMethod]
        public void method1()
        {
        }
    }
}
