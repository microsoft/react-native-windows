using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Reactive.Linq;
using System.Reactive.Threading.Tasks;

namespace ReactNative.Modules.Image
{
    class ImageLoaderModule : NativeModuleBase
    {
        private const string ErrorInvalidUri = "E_INVALID_URI";
        private const string ErrorPrefetchFailure = "E_PREFETCH_FAILURE";
        private const string ErrorGetSizeFailure = "E_GET_SIZE_FAILURE";

        private readonly IImageCache _imageCache;
        private readonly IUriLoader _uriLoader;

        public ImageLoaderModule(IImageCache imageCache, IUriLoader uriLoader)
        {
            _imageCache = imageCache;
            _uriLoader = uriLoader;
        }

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

            try
            {
                await _uriLoader.PrefetchAsync(uriString).ConfigureAwait(false);
                promise.Resolve(true);
            }
            catch (System.Exception ex)
            {
                promise.Reject(ErrorPrefetchFailure, ex.Message);
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

            using (var reference = _imageCache.Get(uriString))
            {
                try
                {
                    await reference.LoadedObservable.FirstAsync().ToTask().ConfigureAwait(false);
                    DispatcherHelpers.RunOnDispatcher(() =>
                        promise.Resolve(new JObject
                        {
                            { "width", reference.Image.PixelWidth },
                            { "height", reference.Image.PixelHeight },
                        }));
                }
                catch (Exception ex)
                {
                    promise.Reject(ErrorGetSizeFailure, ex.Message);
                }
            }
        }
    }
}
