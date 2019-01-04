// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using FBCore.Common.References;
using FBCore.DataSource;
using ImagePipeline.Core;
using ImagePipeline.Image;
using ImagePipeline.Request;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Network;
using System;
using System.IO;
using System.Threading.Tasks;
using static System.FormattableString;

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
        public async void prefetchImage(string uriString, int requestId, IPromise promise)
        {
            if (string.IsNullOrEmpty(uriString))
            {
                promise.Reject(ErrorInvalidUri, "Cannot prefetch an image for an empty URI.");
                return;
            }

            try
            {
                var imagePipeline = ImagePipelineFactory.Instance.GetImagePipeline();
                var uri = new Uri(uriString);

                await _prefetchRequests.AddAndInvokeAsync(
                        requestId, 
                        async token => await imagePipeline.PrefetchToDiskCacheAsync(uri, token).ConfigureAwait(false))
                    .ConfigureAwait(false);

                promise.Resolve(true);
            }
            catch (Exception ex)
            {
                promise.Reject(ErrorPrefetchFailure, ex.Message);
            }
        }

        [ReactMethod]
        public async void prefetchImageAndGetCachedPath(string uriString, int requestId, IPromise promise)
        {
            if (string.IsNullOrEmpty(uriString))
            {
                promise.Reject(ErrorInvalidUri, "Cannot prefetch an image for an empty URI.");
                return;
            }

            try
            {
                var imagePipeline = ImagePipelineFactory.Instance.GetImagePipeline();
                var uri = new Uri(uriString);

                await _prefetchRequests.AddAndInvokeAsync(
                        requestId,
                        async token =>
                            await imagePipeline.PrefetchToDiskCacheAsync(uri, token).ConfigureAwait(false))
                    .ConfigureAwait(false);

                FileInfo file = await imagePipeline.GetFileCachePath(uri).ConfigureAwait(false);
                promise.Resolve(file.FullName);
            }
            catch (Exception ex)
            {
                promise.Reject(ErrorPrefetchFailure, ex.Message);
            }
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

            var uri = new Uri(uriString);
            var imagePipeline = ImagePipelineFactory.Instance.GetImagePipeline();
            var request = ImageRequestBuilder.NewBuilderWithSource(uri).Build();
            var dataSource = imagePipeline.FetchDecodedImage(request, null);
            var dataSubscriber = new BaseDataSubscriberImpl<CloseableReference<CloseableImage>>(
                response =>
                {
                    if (!response.IsFinished())
                    {
                        return Task.CompletedTask;
                    }

                    CloseableReference<CloseableImage> reference = response.GetResult();
                    if (reference != null)
                    {
                        try
                        {
                            CloseableImage image = reference.Get();
                            promise.Resolve(new JObject
                            {
                                { "width", image.Width },
                                { "height", image.Height },
                            });
                        }
                        catch (Exception ex)
                        {
                            promise.Reject(ErrorGetSizeFailure, ex.Message);
                        }
                        finally
                        {
                            CloseableReference<CloseableImage>.CloseSafely(reference);
                        }
                    }
                    else
                    {
                        promise.Reject(ErrorGetSizeFailure, Invariant($"Invalid URI '{uri}' provided."));
                    }

                    return Task.CompletedTask;
                },
                response =>
                {
                    promise.Reject(ErrorGetSizeFailure, response.GetFailureCause());
                });

            dataSource.Subscribe(dataSubscriber, FBCore.Concurrency.CallerThreadExecutor.Instance);
        }

        [ReactMethod]
        public async void queryCache(string[] urls, IPromise promise)
        {
            var imagePipeline = ImagePipelineFactory.Instance.GetImagePipeline();
            var result = new JObject();
            foreach (var url in urls)
            {
                var uri = new Uri(url);
                if (imagePipeline.IsInEncodedMemoryCache(uri))
                {
                    result.Add(url, "memory");
                }
                else if (await imagePipeline.IsInDiskCacheAsync(uri).ConfigureAwait(false))
                {
                    result.Add(url, "disk");
                }
            }

            promise.Resolve(result);
        }

        public override Task OnReactInstanceDisposeAsync()
        {
            _prefetchRequests.CancelAllTasks();
            return Task.CompletedTask;
        }
    }
}
