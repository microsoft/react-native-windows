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

        public static IObservable<ImageStatusEventData> GetStreamLoadObservable(this BitmapImage image)
        {
            return image.GetOpenedObservable()
                .Merge(image.GetFailedObservable(), Scheduler.Default)
                .StartWith(new ImageStatusEventData(ImageLoadStatus.OnLoadStart));
        }
  
        private static IObservable<ImageStatusEventData> GetOpenedObservable(this BitmapImage image)
        {
            return Observable.FromEventPattern<RoutedEventHandler, RoutedEventArgs>(
                h => image.ImageOpened += h,
                h => image.ImageOpened -= h)
                .Select(args =>
                {
                    var bitmapImage = args.Sender as BitmapImage;
                    if (bitmapImage != null)
                    {
                        return new ImageStatusEventData(
                            ImageLoadStatus.OnLoad,
                            new ImageMetadata(
                                image.UriSource?.AbsoluteUri,
                                image.PixelWidth,
                                image.PixelHeight));
                    }
                    else
                    {
                        return new ImageStatusEventData(ImageLoadStatus.OnLoad);
                    }
                })
                .Take(1)
                .Concat(Observable.Return(new ImageStatusEventData(ImageLoadStatus.OnLoadEnd)));
        }

        private static IObservable<ImageStatusEventData> GetFailedObservable(this BitmapImage image)
        {
            return Observable.FromEventPattern<ExceptionRoutedEventHandler, ExceptionRoutedEventArgs>(
                h => image.ImageFailed += h,
                h => image.ImageFailed -= h)
                .Select<EventPattern<ExceptionRoutedEventArgs>, ImageStatusEventData>(pattern =>
                {
                    throw new InvalidOperationException(pattern.EventArgs.ErrorMessage);
                });
        }
    }
}
