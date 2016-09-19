using System;
using System.IO;
using System.Reactive;
using System.Reactive.Concurrency;
using System.Reactive.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Storage.Streams;
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
    }
}
