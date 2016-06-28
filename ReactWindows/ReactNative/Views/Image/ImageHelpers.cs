using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Views.Image
{
    class ImageHelpers
    {
        private const string IMAGE_FOLDER = "ImageCache";

        /// <summary>
        /// Run action on UI thread.
        /// </summary>
        /// <param name="action">The action.</param>
        public static async void RunOnDispatcher(DispatchedHandler action)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, action);
        }

        public static async void SetPixels(
            Border view,
            ImageReference image,
            Color? tintColor,
            Color? backgroundColor)
        {
            var pixels = new byte[image.PixelData.Length];
            image.PixelData.CopyTo(pixels, 0);

            for (var i = 3; i < pixels.Length; i += 4) // 0 = B, 1 = G, 2 = R, 3 = A
            {
                if (tintColor != null)
                {
                    if (pixels[i] != 0)
                    {
                        pixels[i - 3] = (byte)tintColor?.B;
                        pixels[i - 2] = (byte)tintColor?.G;
                        pixels[i - 1] = (byte)tintColor?.R;
                    }
                }

                if (backgroundColor != null)
                {
                    var frontAlpha = (double)pixels[i] / 255;
                    var backAlpha = (double)backgroundColor?.A / 255;

                    pixels[i - 3] = ColorValue(pixels[i - 3], frontAlpha, (double)backgroundColor?.B, backAlpha);
                    pixels[i - 2] = ColorValue(pixels[i - 2], frontAlpha, (double)backgroundColor?.G, backAlpha);
                    pixels[i - 1] = ColorValue(pixels[i - 1], frontAlpha, (double)backgroundColor?.R, backAlpha);
                    pixels[i] = AlphaValue(pixels[i], (double)backgroundColor?.A);
                }
            }

            var writableBitmap = new WriteableBitmap(image.Image.PixelWidth, image.Image.PixelHeight);

            // Open a stream to copy the image contents to the WriteableBitmap's pixel buffer 
            using (System.IO.Stream writeStream = writableBitmap.PixelBuffer.AsStream())
            {
                await writeStream.WriteAsync(pixels, 0, pixels.Length);
            }

            ((ImageBrush)view.Background).ImageSource = writableBitmap;

            image.Dispose();
        }

        public static async void ClearCache()
        {
            var localFolder = await ApplicationData.Current.LocalFolder.TryGetItemAsync(IMAGE_FOLDER);

            if (localFolder == null)
            {
                return;
            }
            else
            {
                await localFolder.DeleteAsync();
            }
        }

        public static string GenerateKeyName(string uriString)
        {
            if (uriString == null)
            {
                return null;
            }

            var index = uriString.IndexOf('?');
            if (index < 4)
            {
                return uriString;
            }
            else
            {
                if (uriString.Substring(index - 3, 3).Equals("PHP", StringComparison.OrdinalIgnoreCase))
                {
                    return uriString;
                }
                else
                {
                    return uriString.Substring(0, index);
                }
            }
        }

        public static string GenerateFileName(string uriString)
        {
            var key = GenerateKeyName(uriString);
            var pattern = new Regex("[/:?\\*<>|\"]");
            return pattern.Replace(key, "_");
        }

        public static void ResolveSize(IPromise promise, BitmapImage image)
        {
            var sizes = new JObject()
            {
                new JProperty("width", image.PixelWidth),
                new JProperty("height",image.PixelHeight),
            };

            promise.Resolve(sizes);
        }

        public static async Task<IRandomAccessStream> OpenAsync(string uriString)
        {
            var localFolder = await ApplicationData.Current.LocalFolder.CreateFolderAsync(
                IMAGE_FOLDER,
                CreationCollisionOption.OpenIfExists);

            var fileName = ImageHelpers.GenerateFileName(uriString);
            var file = await (localFolder as StorageFolder).TryGetItemAsync(fileName);

            if (file != null)
            {
                try
                {
                    return await (file as StorageFile).OpenAsync(FileAccessMode.Read);
                }
                catch (UnauthorizedAccessException)
                {
                    var uri = new Uri(uriString);
                    return await DownloadAsync(uri);
                }            
            }
            else
            {
                var uri = new Uri(uriString);
                ImageHelpers.SaveAsync(uri, fileName);
                return await DownloadAsync(uri);
            }    
        }

        private static async Task<IRandomAccessStream> DownloadAsync(Uri uri)
        {
            var randomAccessStreamReference = RandomAccessStreamReference.CreateFromUri(uri);
            return await randomAccessStreamReference.OpenReadAsync();
        }

        private static async void SaveAsync(Uri uri, string fileName)
        {
            try
            {
                var localFolder = await ApplicationData.Current.LocalFolder.CreateFolderAsync(
                    IMAGE_FOLDER,
                    CreationCollisionOption.OpenIfExists);

                var file = localFolder.CreateFileAsync(
                    fileName,
                    CreationCollisionOption.FailIfExists).AsTask().Result;

                var stream = await DownloadAsync(uri);          

                using (var targetStream = await file.OpenAsync(FileAccessMode.ReadWrite))
                using (var reader = new DataReader(stream.GetInputStreamAt(0)))
                {
                    var output = targetStream.GetOutputStreamAt(0);
                    await reader.LoadAsync((uint)stream.Size);

                    var buffer = reader.ReadBuffer((uint)stream.Size);
                    await output.WriteAsync(buffer);
                    await output.FlushAsync();
                }
            }
            catch (Exception) { }
        }

        private static byte ColorValue(double frontColor, double frontAlpha, double backColor, double backAlpha)
        {
            return (byte)((frontColor * frontAlpha + backColor * backAlpha * (1 - frontAlpha)) /
                                (frontAlpha + backAlpha * (1 - frontAlpha)));
        }

        private static byte AlphaValue(double frontAlpha, double backAlpha)
        {
            return (byte)(frontAlpha + backAlpha * (1 - (frontAlpha / 255)));
        }
    }
}
