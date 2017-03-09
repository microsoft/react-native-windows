using Microsoft.Toolkit.Uwp.UI;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Network;
using System;
using System.Reactive.Linq;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Modules.Image
{
    class ImageLoaderModule : NativeModuleBase
    {
        private const string ErrorInvalidUri = "E_INVALID_URI";
        private const string ErrorPrefetchFailure = "E_PREFETCH_FAILURE";
        private const string ErrorGetSizeFailure = "E_GET_SIZE_FAILURE";

        private readonly TaskCancellationManager<int> _prefetchRequests = new TaskCancellationManager<int>();

        public override string Name
        {
            get
            {
                return "ImageLoader";
            }
        }

        [ReactMethod]
        public void prefetchImage(string uriString, int requestId, IPromise promise)
        {
            if (string.IsNullOrEmpty(uriString))
            {
                promise.Reject(ErrorInvalidUri, "Cannot prefetch an image for an empty URI.");
                return;
            }

            DispatcherHelpers.RunOnDispatcher(async () =>
            {
                try
                {
                    var uri = new Uri(uriString);

                    await _prefetchRequests.AddAndInvokeAsync(
                            requestId, 
                            async token => await ImageCache.Instance.PreCacheAsync(uri, true, true, token).ConfigureAwait(false))
                        .ConfigureAwait(false);

                    promise.Resolve(true);
                }
                catch (Exception ex)
                {
                    promise.Reject(ErrorPrefetchFailure, ex.Message);
                }
            });
        }

        [ReactMethod]
        public void abortRequest(int requestId)
        {
            _prefetchRequests.Cancel(requestId);
        }

        [ReactMethod]
        public void getSize(string uriString, IPromise promise)
        {
            if (string.IsNullOrEmpty(uriString))
            {
                promise.Reject(ErrorInvalidUri, "Cannot get the size of an image for an empty URI.");
                return;
            }

            DispatcherHelpers.RunOnDispatcher(async () =>
            {
                try
                {
                    if (BitmapImageHelpers.IsHttpUri(uriString))
                    {
                        var bitmapImage = await ImageCache.Instance.GetFromCacheAsync(new Uri(uriString), true);
                        promise.Resolve(new JObject
                        {
                            { "width", bitmapImage.PixelWidth },
                            { "height", bitmapImage.PixelHeight },
                        });
                    }
                    else
                    {
                        var bitmapImage = new BitmapImage();
                        var loadQuery = bitmapImage.GetStreamLoadObservable()
                            .Where(status => status.LoadStatus == ImageLoadStatus.OnLoadEnd)
                            .FirstAsync()
                            .Replay(1);

                        using (loadQuery.Connect())
                        {
                            using (var stream = await BitmapImageHelpers.GetStreamAsync(uriString))
                            {
                                await bitmapImage.SetSourceAsync(stream);
                            }

                            await loadQuery;

                            promise.Resolve(new JObject
                            {
                                { "width", bitmapImage.PixelWidth },
                                { "height", bitmapImage.PixelHeight },
                            });
                        }
                    }
                }
                catch (Exception ex)
                {
                    promise.Reject(ErrorGetSizeFailure, ex.Message);
                }
            });
        }

        [ReactMethod]
        public async void queryCache(string[] urls, IPromise promise)
        {
            // TODO: support query for items in memory
            var result = new JObject();
            foreach (var url in urls)
            {
                var file = await ImageCache.Instance.GetFileFromCacheAsync(new Uri(url)).ConfigureAwait(false);
                if (file != null)
                {
                    result.Add(url, "disk");
                }
            }

            promise.Resolve(result);
        }

        public override void OnReactInstanceDispose()
        {
            _prefetchRequests.CancelAllTasks();
        }
    }
}
