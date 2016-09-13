using System;
using System.IO;
using System.Reactive;
using System.Reactive.Concurrency;
using System.Reactive.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;
using static System.FormattableString;

namespace ReactNative.Modules.Image
{
    static class BitmapImageHelpers
    {
        public static bool IsBase64Uri(string uri)
        {
            if (uri == null)
            {
                throw new ArgumentNullException(nameof(uri));
            }

            return uri.StartsWith("data:");
        }

        public static async Task<IRandomAccessStream> GetStreamAsync(string uri)
        {
            if (uri == null)
            {
                throw new ArgumentNullException(nameof(uri));
            }

            if (IsBase64Uri(uri))
            {
                var decodedData = Convert.FromBase64String(uri.Substring(uri.IndexOf(",") + 1));
                return decodedData.AsBuffer().AsStream().AsRandomAccessStream();
            }
            else
            {
                var uriValue = default(Uri);
                if (!Uri.TryCreate(uri, UriKind.Absolute, out uriValue))
                {
                    throw new ArgumentOutOfRangeException(nameof(uri), Invariant($"Invalid URI '{uri}' provided."));
                }

                var streamReference = RandomAccessStreamReference.CreateFromUri(uriValue);
                return await streamReference.OpenReadAsync();
            }
        }

        public static IObservable<ImageLoadStatus> GetStreamLoadObservable(this BitmapImage image)
        {
            return image.GetOpenedObservable()
                .Merge(image.GetFailedObservable(), Scheduler.Default)
                .StartWith(ImageLoadStatus.OnLoadStart);
        }

        public static IObservable<ImageLoadStatus> GetUriLoadObservable(this BitmapImage image)
        {
            return Observable.Merge(
                Scheduler.Default,
                image.GetDownloadingObservable(),
                image.GetOpenedObservable(),
                image.GetFailedObservable());
        }

        private static IObservable<ImageLoadStatus> GetOpenedObservable(this BitmapImage image)
        {
            return Observable.FromEventPattern<RoutedEventHandler, RoutedEventArgs>(
                h => image.ImageOpened += h,
                h => image.ImageOpened -= h)
                .Select(_ => ImageLoadStatus.OnLoad)
                .Take(1)
                .Concat(Observable.Return(ImageLoadStatus.OnLoadEnd));
        }

        private static IObservable<ImageLoadStatus> GetFailedObservable(this BitmapImage image)
        {
            return Observable.FromEventPattern<ExceptionRoutedEventHandler, ExceptionRoutedEventArgs>(
                h => image.ImageFailed += h,
                h => image.ImageFailed -= h)
                .Select<EventPattern<ExceptionRoutedEventArgs>, ImageLoadStatus>(pattern =>
                {
                    throw new InvalidOperationException(pattern.EventArgs.ErrorMessage);
                });
        }

        private static IObservable<ImageLoadStatus> GetDownloadingObservable(this BitmapImage image)
        {
            return Observable.FromEventPattern<DownloadProgressEventHandler, DownloadProgressEventArgs>(
                h => image.DownloadProgress += h,
                h => image.DownloadProgress -= h)
                .Take(1)
                .Select(_ => ImageLoadStatus.OnLoadStart);
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

        public static async void ColorizePixelData(
            Windows.UI.Xaml.Controls.Border view,
            uint width,
            uint height,
            byte[] pixelData,
            Color? tintColor,
            Color? backgroundColor)
        {
            var pixels = new byte[pixelData.Length];
            pixelData.CopyTo(pixels, 0);

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

            var writableBitmap = new WriteableBitmap((int)width, (int)height);

            // Open a stream to copy the image contents to the WriteableBitmap's pixel buffer 
            using (System.IO.Stream writeStream = writableBitmap.PixelBuffer.AsStream())
            {
                await writeStream.WriteAsync(pixels, 0, pixels.Length);
            }

            ((Windows.UI.Xaml.Media.ImageBrush)view.Background).ImageSource = writableBitmap;
        }
    }
}
