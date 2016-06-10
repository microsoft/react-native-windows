using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Views.Image;
using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Core;
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

        private readonly Dictionary<string, BitmapImage> _images =
            new Dictionary<string, BitmapImage>();

        private readonly Dictionary<string, byte[]> _imagePixelData =
            new Dictionary<string, byte[]>();

        private readonly Dictionary<string, List<Tuple<Border, Color?, Color?>>> _waitingListImage =
            new Dictionary<string, List<Tuple<Border, Color?, Color?>>>();

        private readonly Dictionary<string, List<Tuple<Border, Color?, Color?>>> _waitingListPixelData =
            new Dictionary<string, List<Tuple<Border, Color?, Color?>>>();

        private readonly Dictionary<string, Tuple<int, bool>> _imageReferences =
            new Dictionary<string, Tuple<int, bool>>();

        public override string Name
        {
            get
            {
                return "ImageLoader";
            }
        }

        [ReactMethod]
        public void prefetchImage(string uriString, IPromise promise)
        {
            var image = GetImage(uriString);
            if (image != null)
            {
                ImageReference(uriString, 1, true);
                promise.Resolve(true);
                return;
            }

            RunOnDispatcher(() =>
            {
                var imageFailedHandler = new ExceptionRoutedEventHandler(
                    (sender, args) => OnImageLoadFailedWithPromise(sender, args, promise));

                var imageOpenedHandler = new RoutedEventHandler(
                    (sender, args) => OnImageLoadedWithPromise(sender, args, false, uriString, promise));

                try
                {
                    var uri = new Uri(uriString);
                    Load(imageOpenedHandler, imageFailedHandler, uri);
                }
                catch (Exception ex) when (ex is ArgumentNullException || ex is UriFormatException)
                {
                    promise.Reject(ex.Message);
                }
            });
        }

        [ReactMethod]
        public void getSize(string uriString, IPromise promise)
        {
            var image = GetImage(uriString);
            if (image != null)
            {
                var sizes = new JObject()
                {
                    new JProperty("width", image.PixelWidth),
                    new JProperty("height",image.PixelHeight),
                };

                promise.Resolve(sizes);
                return;
            }

            RunOnDispatcher(() =>
            {
                var imageFailedHandler = new ExceptionRoutedEventHandler(
                    (sender, args) => OnImageLoadFailedWithPromise(sender, args, promise));

                var imageOpenedHandler = new RoutedEventHandler(
                    (sender, args) => OnImageLoadedWithPromise(sender, args, true, uriString, promise));

                try
                {
                    var uri = new Uri(uriString);
                    Load(imageOpenedHandler, imageFailedHandler, uri);
                }
                catch (Exception ex) when (ex is ArgumentNullException || ex is UriFormatException)
                {
                    promise.Reject(ex.Message);
                }
            });
        }

        internal BitmapImage QueryImage(string uriString)
        {
            var image = GetImage(uriString);

            if (image != null)
            {
                ImageReference(uriString, 1);
            }

            return image;
        }

        internal void FreeReference(string uriString)
        {
            ImageReference(uriString, -1);
        }

        internal BitmapImage GetImage(string uriString)
        {
            if (uriString == null)
            {
                return null;
            }

            lock(_images)
            {
                var image = default(BitmapImage);

                if (_images.TryGetValue(uriString, out image))
                {
                    return image;
                }
                else
                {
                    return null;
                }
            }         
        }

        internal byte[] GetPixelData(string uriString)
        {
            lock(_imagePixelData)
            {
                var data = default(byte[]);

                if (_imagePixelData.TryGetValue(uriString, out data))
                {
                    return data;
                }
                else
                {
                    return null;
                }
            }         
        }

        internal void LoadImage(string uriString, ReactImageManager manager, object data)
        {
            if (Waiting(_waitingListImage, uriString, data))
            {
                return;
            }

            var imageFailedHandler = new ExceptionRoutedEventHandler(
                (sender, args) => OnImageLoadFailed(sender, args, uriString, manager));

            var imageOpenedHandler = new RoutedEventHandler(
                (sender, args) => OnImageLoaded(sender, args, uriString, manager));

            try
            {
                var uri = new Uri(uriString);
                Load(imageOpenedHandler, imageFailedHandler, uri);
            }
            catch (Exception ex) when (ex is ArgumentNullException || ex is UriFormatException)
            {
                OnImageLoadFailed(null, null, uriString, manager);
            }
        }

        internal async void LoadImagePixelData(string uriString, ReactImageManager manager, object data)
        {
            if (Waiting(_waitingListPixelData, uriString, data))
            {
                return;
            }

            var randomAccessStreamReference = RandomAccessStreamReference.CreateFromUri(new Uri(uriString));
            var randomAccessStream = await randomAccessStreamReference.OpenReadAsync();
            var decoder = await BitmapDecoder.CreateAsync(randomAccessStream);

            var pixelData = await decoder.GetPixelDataAsync(
                BitmapPixelFormat.Bgra8, // WriteableBitmap uses BGRA format 
                BitmapAlphaMode.Straight,
                new BitmapTransform(),
                ExifOrientationMode.RespectExifOrientation,
                ColorManagementMode.DoNotColorManage
            );

            lock(_imagePixelData)
            {
                _imagePixelData[uriString] = pixelData.DetachPixelData();
            }
                
            lock (_waitingListPixelData)
            {
                foreach (var item in _waitingListPixelData[uriString])
                {
                    manager.DataLoaded(item, uriString);
                }

                _waitingListPixelData.Remove(uriString);
            }
        }

        private void OnImageLoadFailedWithPromise(object sender, ExceptionRoutedEventArgs args, IPromise promise)
        {
            RemoveEventHandlers(sender as BitmapImage);
            promise.Reject(args.ErrorMessage);
        }

        private void OnImageLoadedWithPromise(object sender, RoutedEventArgs args, bool size, string uriString, IPromise promise)
        {
            var image = sender as BitmapImage;
            RemoveEventHandlers(image);
          
            if (size)
            {
                var sizes = new JObject()
                {
                    new JProperty("width", image.PixelWidth),
                    new JProperty("height",image.PixelHeight),
                };

                promise.Resolve(sizes);
            }
            else
            {
                ImageReference(uriString, 1, true);
                promise.Resolve(true);
            }

            lock (_images)
            {
                _images[uriString] = image;
            }
        }

        private void OnImageLoadFailed(object sender, ExceptionRoutedEventArgs args, string uriString, ReactImageManager manager)
        {
            lock(_waitingListImage)
            {                              
                RemoveEventHandlers(sender as BitmapImage);

                foreach (var item in _waitingListImage[uriString])
                {
                    manager.ImageLoaded(item, false, uriString);
                }

                _waitingListImage.Remove(uriString);
            }
        }

        private void OnImageLoaded(object sender, RoutedEventArgs args, string uriString,  ReactImageManager manager)
        {
            lock (_waitingListImage)
            {
                var image = sender as BitmapImage;
                RemoveEventHandlers(image);

                ImageReference(uriString, _waitingListImage[uriString].Count, false);

                lock (_images)
                {
                    _images[uriString] = image;
                }               

                foreach (var item in _waitingListImage[uriString])
                {                
                    manager.ImageLoaded(item, true, uriString);
                }

                _waitingListImage.Remove(uriString);
            }
        }

        private void RemoveEventHandlers(BitmapImage image)
        {
            if (image == null)
            {
                return;
            }

            lock(_imageFailedHandlers)
            {
                var imageFailedHandler = default(ExceptionRoutedEventHandler);
                if (_imageFailedHandlers.TryGetValue(image, out imageFailedHandler))
                {
                    _imageFailedHandlers.Remove(image);
                    image.ImageFailed -= imageFailedHandler;
                }
            }
            
            lock(_imageOpenedHandlers)
            {
                var imageOpenedHandler = default(RoutedEventHandler);
                if (_imageOpenedHandlers.TryGetValue(image, out imageOpenedHandler))
                {
                    _imageOpenedHandlers.Remove(image);
                    image.ImageOpened -= imageOpenedHandler;
                }
            }         
        }

        private bool Waiting(Dictionary<string, List<Tuple<Border, Color?, Color?>>> waitList, string uriString, object data)
        {
            lock (waitList)
            {
                var list = default(List<Tuple<Border, Color?, Color?>>);

                if (waitList.TryGetValue(uriString, out list))
                {
                    list.Add((Tuple<Border, Color?, Color?>)data);
                    return true;
                }
                else
                {
                    waitList.Add(
                        uriString,
                        new List<Tuple<Border, Color?, Color?>>()
                        {
                            (Tuple<Border, Color?, Color?>)data
                        });

                    return false;
                }
            }
        }

        private void ImageReference(string uriString, int count, bool prefetch = false)
        {
            if (uriString == null)
            {
                return;
            }

            lock (_imageReferences)
            {
                var reference = default(Tuple<int, bool>);

                if (_imageReferences.TryGetValue(uriString, out reference))
                {
                    if (reference.Item1 + count == 0)
                    {
                        lock (_waitingListImage)
                        {
                            lock (_waitingListPixelData)
                            {
                                lock (_images)
                                {
                                    lock (_imagePixelData)
                                    {
                                        _imageReferences.Remove(uriString);
                                        _imagePixelData.Remove(uriString);
                                        _images.Remove(uriString);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        _imageReferences[uriString] = new Tuple<int, bool>(reference.Item1 + count, prefetch || reference.Item2);
                    }
                }
                else
                {
                    _imageReferences[uriString] = new Tuple<int, bool>(count, prefetch);
                }
            }
        }

        private async void Load(RoutedEventHandler handler, ExceptionRoutedEventHandler errorHandler, Uri uri)
        {
            var image = new BitmapImage();

            image.ImageFailed += errorHandler;
            image.ImageOpened += handler;

            lock(_imageFailedHandlers)
            {
                _imageFailedHandlers.Add(image, errorHandler);
            }
            
            lock(_imageOpenedHandlers)
            {
                _imageOpenedHandlers.Add(image, handler);
            }
            
            var randomAccessStreamReference = RandomAccessStreamReference.CreateFromUri(uri);
            var randomAccessStream = await randomAccessStreamReference.OpenReadAsync();

            await image.SetSourceAsync(randomAccessStream);
        }

        /// <summary>
        /// Run action on UI thread.
        /// </summary>
        /// <param name="action">The action.</param>
        private static async void RunOnDispatcher(DispatchedHandler action)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, action);
        }    
    }
}
