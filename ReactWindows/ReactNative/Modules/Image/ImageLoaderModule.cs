using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Views.Image;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Graphics.Imaging;
using Windows.Storage;
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
        private static string IMAGE_FOLDER = "ImageCache";

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
                promise.Resolve(true);
                return;
            }

            if (Waiting(uriString, _waitingListImage, null, Tuple.Create<IPromise, bool>(promise, false)))
            {
                return;
            }
            else
            {
                RunOnDispatcher(() =>
                {
                    LoadAsync(uriString);
                });
            }
        }

        [ReactMethod]
        public void getSize(string uriString, IPromise promise)
        {
            var image = GetImage(uriString);

            if (image != null)
            {
                var sizes = new JObject()
                {
                    new JProperty("width", image.Image.PixelWidth),
                    new JProperty("height",image.Image.PixelHeight),
                };

                promise.Resolve(sizes);
                return;
            }

            if (Waiting(uriString, _waitingListImage, null, Tuple.Create<IPromise, bool>(promise, true)))
            {
                return;
            }
            else
            {
                RunOnDispatcher(() =>
                {
                    LoadAsync(uriString);
                });
            }
        }

        internal ImageReference QueryImage(string uriString)
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

                if (_images.TryGetValue(GenerateName(uriString), out image))
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
            if (Waiting(uriString, _waitingListImage, data))
            {
                return;
            }
            else
            {
                LoadAsync(uriString);
            }
        }

        internal async void LoadPixelDataAsync(string uriString, object data)
        {
            if (Waiting(uriString, _waitingListPixelData, data)) return;

            IRandomAccessStreamWithContentType stream = null;

            try
            {
                stream = await OpenAsync(GenerateName(uriString));
            }
            catch (UnauthorizedAccessException) // Saving in progress
            {
                var randomAccessStreamReference = RandomAccessStreamReference.CreateFromUri(new Uri(uriString));
                stream = await randomAccessStreamReference.OpenReadAsync();
            }

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
                _images[uriString].SetPixelData(pixelData.DetachPixelData());
                foreach (var item in _waitingListPixelData[uriString])
                {
                    var manager = ((Tuple<ReactImageManager, Border, Color?, Color?>)item.Item1).Item1;
                    manager.DataLoaded(item.Item1, uriString);
                }

                _waitingListPixelData.Remove(uriString);
            }
        }

        private void OnImageLoadFailed(object sender, ExceptionRoutedEventArgs args, string uriString)
        {
            lock(_images)
            {                              
                RemoveEventHandlers(sender as BitmapImage);

                foreach (var item in _waitingListImage[uriString])
                {
                    if (item.Item1 != null)
                    {
                        var manager = ((Tuple<ReactImageManager, Border, Color?, Color?>)item.Item1).Item1;
                        manager.ImageLoaded(item.Item1, false, uriString);
                    }
                    else
                    {
                        var promise = ((Tuple<IPromise, bool>)item.Item2).Item1;
                        if (args != null)
                        {
                            promise.Reject(args.ErrorMessage);
                        }
                        else
                        {
                            promise.Reject(sender as string);
                        }
                    }
                }

                _waitingListImage.Remove(uriString);
            }
        }

        private void OnImageLoaded(object sender, RoutedEventArgs args, string uriString)
        {
            lock (_images)
            {
                var image = sender as BitmapImage;
                RemoveEventHandlers(image);

                _images[uriString] = new ImageReference(image, _waitingListImage[uriString].Count, uriString, _images);              

                foreach (var item in _waitingListImage[uriString])
                {

                    if (item.Item1 != null)
                    {
                        var manager = ((Tuple<ReactImageManager, Border, Color?, Color?>)item.Item1).Item1;
                        manager.ImageLoaded(item.Item1, true, uriString);
                    }
                    else
                    {
                        var promise = ((Tuple<IPromise, bool>)item.Item2).Item1;
                        var size = ((Tuple<IPromise, bool>)item.Item2).Item2;

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
                            promise.Resolve(true);
                        }
                    }
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
            lock(_images)
            {
                var name = GenerateName(uriString);                
                var list = default(List<Tuple<object, object>>);

                if (waitList.TryGetValue(name, out list))
                {
                    list.Add(Tuple.Create<object, object>(view, promise));
                                           
                    return true;
                }
                else
                {
                    waitList.Add(
                        name,
                        new List<Tuple<object, object>>()
                        {
                            Tuple.Create<object, object>(view, promise)
                        });

                    return false;
                }
            }
        }

        private async void LoadAsync(string uriString)
        {
            try
            {
                var name = GenerateName(uriString);                
                var uri = new Uri(uriString);

                var imageFailedHandler = new ExceptionRoutedEventHandler(
                    (sender, args) => OnImageLoadFailed(sender, args, name));

                var imageOpenedHandler = new RoutedEventHandler(
                    (sender, args) => OnImageLoaded(sender, args, name));

                var image = new BitmapImage();

                image.ImageFailed += imageFailedHandler;
                image.ImageOpened += imageOpenedHandler;

                lock(_images)
                {
                    _imageFailedHandlers.Add(image, imageFailedHandler);   
                    _imageOpenedHandlers.Add(image, imageOpenedHandler);
                }

                var randomAccessStream = await OpenAsync(name);

                if (randomAccessStream == null)
                {
                    var randomAccessStreamReference = RandomAccessStreamReference.CreateFromUri(uri);
                    randomAccessStream = await randomAccessStreamReference.OpenReadAsync();
                    await SaveAsync(randomAccessStream, name);
 
                    randomAccessStream.Seek(0);
                    await image.SetSourceAsync(randomAccessStream);
                }
                else
                {
                    await image.SetSourceAsync(randomAccessStream);
                }              
            }
            catch (Exception ex) when (ex is ArgumentNullException || ex is UriFormatException)
            {
                OnImageLoadFailed(ex.Message, null, uriString);
            }
        }

        private async Task SaveAsync(IRandomAccessStreamWithContentType stream, string name)
        {    
            var localFolder = await ApplicationData.Current.LocalFolder.CreateFolderAsync(
                IMAGE_FOLDER, 
                CreationCollisionOption.OpenIfExists);

            var file = localFolder.CreateFileAsync(
                GenerateFileName(name), 
                CreationCollisionOption.ReplaceExisting).AsTask().Result;
            
            using (var targetStream = await file.OpenAsync(FileAccessMode.ReadWrite))
            using (var reader = new DataReader(stream.GetInputStreamAt(0)))
            {
                var output = targetStream.GetOutputStreamAt(0);
                await reader.LoadAsync((uint)stream.Size);

                var buffer = reader.ReadBuffer((uint)stream.Size);
                var bytes = await output.WriteAsync(buffer);
                var flushed = await output.FlushAsync();
            }
        }

        private async Task<IRandomAccessStreamWithContentType> OpenAsync(string name)
        {
            var localFolder = await ApplicationData.Current.LocalFolder.TryGetItemAsync(IMAGE_FOLDER);

            if (localFolder == null)
            {
                return null;
            }

            var file = await (localFolder as StorageFolder)?.TryGetItemAsync(GenerateFileName(name));

            if (file != null)
            {
                return await (file as StorageFile).OpenReadAsync();
            }
            else
            {
                return null;
            }         
        }

        private string GenerateName(string uriString)
        {
            if (uriString != null)
            {
                var split = uriString.Split('?');
                return split[0];
            }
            else
            {
                return null;
            }         
        }

        private string GenerateFileName(string name)
        {
            return name.Replace('/', '_').Replace(':', '_');
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
