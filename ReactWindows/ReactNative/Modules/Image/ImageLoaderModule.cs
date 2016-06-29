using ReactNative.Bridge;
using System.Reactive.Linq;

namespace ReactNative.Modules.Image
{
    class ImageLoaderModule : NativeModuleBase
    {
        private const string ErrorInvalidUri = "E_INVALID_URI";

        private readonly IImageCache _cache = new InMemoryImageCache();

        public override string Name
        {
            get
            {
                return "ImageLoader";
            }
        }

        [ReactMethod]
        public async void prefetchImage(string uriString, IPromise promise)
        {
            if (uriString == null)
            {
                promise.Reject(ErrorInvalidUri, "Cannot prefetch an image for an empty URI.");
                return;
            }

            var r = _cache.Get(uriString);
            try
            {
                await r.LoadedObservable.FirstAsync();
            }
        }
    }
}
