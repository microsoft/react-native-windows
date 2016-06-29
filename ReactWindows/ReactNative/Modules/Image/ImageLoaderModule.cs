using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System.Reactive.Linq;

namespace ReactNative.Modules.Image
{
    class ImageLoaderModule : NativeModuleBase
    {
        private const string ErrorInvalidUri = "E_INVALID_URI";
        private const string ErrorPrefetchFailure = "E_PREFETCH_FAILURE";
        private const string ErrorGetSizeFailure = "E_GET_SIZE_FAILURE";

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
            if (string.IsNullOrEmpty(uriString))
            {
                promise.Reject(ErrorInvalidUri, "Cannot prefetch an image for an empty URI.");
                return;
            }

            var r = _cache.Get(uriString);
            try
            {
                await r.LoadedObservable.FirstAsync();
                promise.Resolve(true);
            }
            catch (ImageFailedException ex)
            {
                promise.Reject(ErrorGetSizeFailure, ex.Message);
            }
        }

        [ReactMethod]
        public async void getSize(string uriString, IPromise promise)
        {
            if (string.IsNullOrEmpty(uriString))
            {
                promise.Reject(ErrorInvalidUri, "Cannot get the size of an image for an empty URI.");
                return;
            }

            var r = _cache.Get(uriString);
            try
            {
                await r.LoadedObservable.FirstAsync();
                promise.Resolve(new JObject
                {
                    { "width", r.Image.PixelWidth },
                    { "height", r.Image.PixelHeight },
                });
            }
            catch (ImageFailedException ex)
            {
                promise.Reject(ErrorPrefetchFailure, ex.Message);
            }
        }
    }
}
