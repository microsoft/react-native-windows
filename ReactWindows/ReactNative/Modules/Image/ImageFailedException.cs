using System;

namespace ReactNative.Modules.Image
{
    class ImageFailedException : Exception
    {
        public ImageFailedException(string message)
            : base(message)
        {
        }
    }
}
