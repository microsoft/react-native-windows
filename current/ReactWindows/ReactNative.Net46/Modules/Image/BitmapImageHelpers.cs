// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Net;
using System.Reactive;
using System.Reactive.Concurrency;
using System.Reactive.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
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

        public static Stream GetStreamAsync(string uri)
        {
            if (uri == null)
            {
                throw new ArgumentNullException(nameof(uri));
            }

            if (IsBase64Uri(uri))
            {
                var decodedData = Convert.FromBase64String(uri.Substring(uri.IndexOf(",") + 1));
                return new MemoryStream(decodedData);
            }
            else
            {
                var uriValue = default(Uri);
                if (!Uri.TryCreate(uri, UriKind.Absolute, out uriValue))
                {
                    throw new ArgumentOutOfRangeException(nameof(uri), Invariant($"Invalid URI '{uri}' provided."));
                }

                var streamReference = new StreamReader(WebRequest.Create(uri).GetResponse().GetResponseStream()); //RandomAccessStreamReference.CreateFromUri(uriValue);
                return streamReference.BaseStream;
            }
        }

        public static IObservable<ImageStatusEventData> GetStreamLoadObservable(this BitmapImage image)
        {
            return image.GetOpenedObservable()
                .Merge(image.GetFailedObservable(), Scheduler.Default)
                .StartWith(new ImageStatusEventData(ImageLoadStatus.OnLoadStart));
        }

        public static IObservable<ImageStatusEventData> GetUriLoadObservable(this BitmapImage image)
        {
            return Observable.Merge(
                Scheduler.Default,
                image.GetDownloadingObservable(),
                image.GetOpenedObservable(),
                image.GetFailedObservable());
        }

        private static IObservable<ImageStatusEventData> GetOpenedObservable(this BitmapImage image)
        {
            return Observable.FromEventPattern<EventHandler, RoutedEventArgs>(
                h => image.DownloadCompleted += h,
                h => image.DownloadCompleted -= h)
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
            return Observable.FromEventPattern<EventHandler<System.Windows.Media.ExceptionEventArgs>, ExceptionRoutedEventArgs>(
                h => image.DownloadFailed += h,
                h => image.DownloadFailed -= h)
                .Select<EventPattern<ExceptionRoutedEventArgs>, ImageStatusEventData>(pattern =>
                {
                    throw new InvalidOperationException(pattern.EventArgs.ErrorException.Message);
                });
        }

        private static IObservable<ImageStatusEventData> GetDownloadingObservable(this BitmapImage image)
        {
            return Observable.FromEventPattern<EventHandler<DownloadProgressEventArgs>, DownloadProgressChangedEventArgs>(
                h => image.DownloadProgress += h,
                h => image.DownloadProgress -= h)
                .Take(1)
                .Select(_ => new ImageStatusEventData(ImageLoadStatus.OnLoadStart));
        }
    }
}
