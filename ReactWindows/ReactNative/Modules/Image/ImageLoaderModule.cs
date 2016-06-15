using ReactNative.Bridge;
using ReactNative.Views.Image;
using System;
using System.Collections.Generic;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Modules.Image
{
    class ImageLoaderModule : NativeModuleBase
    {       
        private readonly Dictionary<BitmapImage, ExceptionRoutedEventHandler> _imageFailedHandlers =
            new Dictionary<BitmapImage, ExceptionRoutedEventHandler>();

        private readonly Dictionary<BitmapImage, RoutedEventHandler> _imageOpenedHandlers =
            new Dictionary<BitmapImage, RoutedEventHandler>();

        private readonly Dictionary<string, ImageReference> _images =
            new Dictionary<string, ImageReference>();

        private readonly Dictionary<string, List<Tuple<object, object>>> _waitingListImage =
            new Dictionary<string, List<Tuple<object, object>>>();

        private readonly Dictionary<string, List<Tuple<object, object>>> _waitingListPixelData =
            new Dictionary<string, List<Tuple<object, object>>>();

        /// <summary>
        /// Instantiates the <see cref="ImageLoaderModule"/>.
        /// </summary>
        internal ImageLoaderModule()
        {
            ImageHelpers.ClearCache();
        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "ImageLoader";
            }
        }

        /// <summary>
        /// Prefetch image from uri to local storage.
        /// </summary>
        /// <param name="uriString">The uri of the image.</param>
        /// <param name="promise">The promise.</param>
        [ReactMethod]
        public void prefetchImage(string uriString, IPromise promise)
        {
            var image = GetImage(uriString);

            if (image != null)
            {
                promise.Resolve(true);
                return;
            }

            lock(_images)
            {
                if (Waiting(uriString, _waitingListImage, null, Tuple.Create<IPromise, bool>(promise, false)))
                {
                    return;
                }
                else
                {
                    ImageHelpers.RunOnDispatcher(() =>
                    {
                        LoadAsync(uriString);
                    });
                }
            }           
        }

        /// <summary>
        /// Get image size from uri.
        /// </summary>
        /// <param name="uriString">The uri of the image.</param>
        /// <param name="promise">The promise.</param>
        [ReactMethod]
        public void getSize(string uriString, IPromise promise)
        {
            var image = GetImage(uriString);

            if (image != null)
            {
                ImageHelpers.ResolveSize(promise, image.Image);
                return;
            }

            lock(_images)
            {
                if (Waiting(uriString, _waitingListImage, null, Tuple.Create<IPromise, bool>(promise, true)))
                {
                    return;
                }
                else
                {
                    ImageHelpers.RunOnDispatcher(() =>
                    {
                        LoadAsync(uriString);
                    });
                }
            }          
        }

        internal ImageReference ReserveImage(string uriString)
        {
            lock(_images)
            {
                var image = GetImage(uriString);

                if (image != null)
                {
                    image.Reserve();
                }

                return image;
            }
        }

        internal ImageReference GetImage(string uriString)
        {
            if (uriString == null)
            {
                return null;
            }

            lock (_images)
            {
                var image = default(ImageReference);

                if (_images.TryGetValue(ImageHelpers.GenerateKeyName(uriString), out image))
                {
                    return image;
                }
                else
                {
                    return null;
                }
            }         
        }

        internal void LoadImage(string uriString, object data)
        {
            lock(_images)
            {
                if (Waiting(uriString, _waitingListImage, data))
                {
                    return;
                }
                else
                {
                    LoadAsync(uriString);
                }
            }        
        }

        internal async void LoadPixelDataAsync(string uriString, object data)
        {
            if (Waiting(uriString, _waitingListPixelData, data)) return;

            var stream = await ImageHelpers.OpenAsync(uriString);
            var decoder = await BitmapDecoder.CreateAsync(stream);

            var pixelData = await decoder.GetPixelDataAsync(
                BitmapPixelFormat.Bgra8, // WriteableBitmap uses BGRA format 
                BitmapAlphaMode.Straight,
                new BitmapTransform(),
                ExifOrientationMode.RespectExifOrientation,
                ColorManagementMode.DoNotColorManage
            );

            lock(_images)
            {
                var key = ImageHelpers.GenerateKeyName(uriString);
                _images[key].SetPixelData(pixelData.DetachPixelData());
                foreach (var item in _waitingListPixelData[key])
                {
                    var manager = ((Tuple<ReactImageManager, Border, Color?, Color?>)item.Item1).Item1;
                    manager.DataLoaded(item.Item1, _images[key]);
                }

                _waitingListPixelData.Remove(key);
            }
        }

        private async void LoadAsync(string uriString)
        {
            var key = ImageHelpers.GenerateKeyName(uriString);
            try
            {
                var uri = new Uri(uriString);              

                var imageFailedHandler = new ExceptionRoutedEventHandler(
                    (sender, args) => OnImageLoadFailed(sender, args, key));

                var imageOpenedHandler = new RoutedEventHandler(
                    (sender, args) => OnImageLoaded(sender, args, key));

                var image = new BitmapImage();

                image.ImageFailed += imageFailedHandler;
                image.ImageOpened += imageOpenedHandler;

                lock (_images)
                {
                    _imageFailedHandlers.Add(image, imageFailedHandler);
                    _imageOpenedHandlers.Add(image, imageOpenedHandler);
                }

                var stream = await ImageHelpers.OpenAsync(uriString);
                await image.SetSourceAsync(stream);
            }
            catch (Exception ex) when (ex is ArgumentNullException || ex is UriFormatException)
            {
                OnImageLoadFailed(ex, null, key);
            }
        }

        private void OnImageLoadFailed(object sender, ExceptionRoutedEventArgs args, string key)
        {
            lock(_images)
            {                              
                RemoveEventHandlers(sender as BitmapImage);

                foreach (var item in _waitingListImage[key])
                {
                    if (item.Item1 != null)
                    {
                        var manager = ((Tuple<ReactImageManager, Border, Color?, Color?>)item.Item1).Item1;
                        manager.ImageLoaded(item.Item1, null);
                    }
                    else
                    {
                        var promise = ((Tuple<IPromise, bool>)item.Item2).Item1;
                        if (args != null)
                        {
                            promise.Reject("EUNSPECIFIED", args.ErrorMessage, null);
                        }
                        else
                        {
                            promise.Reject(sender as Exception);
                        }
                    }
                }

                _waitingListImage.Remove(key);
            }
        }

        private void OnImageLoaded(object sender, RoutedEventArgs args, string key)
        {
            lock (_images)
            {
                var image = sender as BitmapImage;
                RemoveEventHandlers(image);

                var imageReference = new ImageReference(image, _waitingListImage[key].Count, key, _images);
                _images[key] = imageReference;              

                foreach (var item in _waitingListImage[key])
                {
                    if (item.Item1 != null)
                    {
                        var manager = ((Tuple<ReactImageManager, Border, Color?, Color?>)item.Item1).Item1;
                        manager.ImageLoaded(item.Item1, imageReference);
                    }
                    else
                    {
                        var promise = ((Tuple<IPromise, bool>)item.Item2).Item1;
                        var getSize = ((Tuple<IPromise, bool>)item.Item2).Item2;

                        if (getSize)
                        {
                            ImageHelpers.ResolveSize(promise, image);
                        }
                        else
                        {
                            promise.Resolve(true);
                        }
                    }
                }

                _waitingListImage.Remove(key);
            }
        }

        private void RemoveEventHandlers(BitmapImage image)
        {
            if (image == null)
            {
                return;
            }

            lock(_images)
            {
                var imageFailedHandler = default(ExceptionRoutedEventHandler);

                if (_imageFailedHandlers.TryGetValue(image, out imageFailedHandler))
                {
                    _imageFailedHandlers.Remove(image);
                    image.ImageFailed -= imageFailedHandler;
                }

                var imageOpenedHandler = default(RoutedEventHandler);

                if (_imageOpenedHandlers.TryGetValue(image, out imageOpenedHandler))
                {
                    _imageOpenedHandlers.Remove(image);
                    image.ImageOpened -= imageOpenedHandler;
                }
            }         
        }

        private bool Waiting(
            string uriString,
            Dictionary<string, List<Tuple<object, object>>> waitList,
            object view,
            object promise = null)
        {
            var list = default(List<Tuple<object, object>>);

            if (waitList.TryGetValue(ImageHelpers.GenerateKeyName(uriString), out list))
            {
                list.Add(Tuple.Create<object, object>(view, promise));

                return true;
            }
            else
            {
                waitList.Add(
                    ImageHelpers.GenerateKeyName(uriString),
                    new List<Tuple<object, object>>()
                    {
                        Tuple.Create<object, object>(view, promise)
                    });

                return false;
            }
        }         
    }
}
