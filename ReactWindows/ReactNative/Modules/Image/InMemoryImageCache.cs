using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Reactive;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Threading;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Modules.Image
{
    class InMemoryImageCache : IImageCache
    {
        private readonly IDictionary<string, Impl> _cache =
            new Dictionary<string, Impl>();

        public IImageReference Get(string uri)
        {
            var reference = default(Impl);
            lock (_cache)
            {
                if (_cache.TryGetValue(uri, out reference))
                {
                    reference.Increment();
                }
            }

            if (reference == null)
            {
                lock (_cache)
                {
                    if (!_cache.TryGetValue(uri, out reference))
                    {
                        reference = new Impl(this, uri);
                        _cache.Add(uri, reference);
                    }
                    else
                    {
                        reference.Increment();
                    }
                }
            }

            return reference;
        }

        private void Release(Impl reference)
        {
            _cache.Remove(reference.Uri);
        }

        class Impl : IImageReference
        {
            private readonly InMemoryImageCache _parent;
            private readonly IDisposable _disposable;
            private int _refCount = 1;

            public Impl(InMemoryImageCache parent, string uri)
            {
                _parent = parent;
                Uri = uri;

                Image = new BitmapImage();

                var io = CreateObservable();
                LoadedObservable = io;
                _disposable = io.Connect();

                InitializeImage();
            }

            public string Uri
            {
                get;
            }

            public BitmapImage Image
            {
                get;
            }

            public IObservable<Unit> LoadedObservable
            {
                get;
            }

            public IObservable<string> FailedObservable
            {
                get;
            }

            public void Increment()
            {
                Interlocked.Increment(ref _refCount);
            }

            public void Dispose()
            {
                lock (_parent._cache)
                {
                    if (Interlocked.Decrement(ref _refCount) == 0)
                    {
                        _disposable.Dispose();
                        _parent.Release(this);
                    }
                }
            }

            private IConnectableObservable<Unit> CreateObservable()
            {
                var openedObservable = Observable.FromEventPattern<RoutedEventHandler, RoutedEventArgs>(
                    h => Image.ImageOpened += h,
                    h => Image.ImageOpened -= h)
                    .Select(_ => default(Unit));

                var failedObservable = Observable.FromEventPattern<ExceptionRoutedEventHandler, ExceptionRoutedEventArgs>(
                    h => Image.ImageFailed += h,
                    h => Image.ImageFailed -= h)
                    .Select<EventPattern<ExceptionRoutedEventArgs>, Unit>(pattern =>
                    {
                        throw new Exception(pattern.EventArgs.ErrorMessage);
                    });

                return openedObservable
                    .Merge(failedObservable)
                    .Replay(1);
            }

            private async void InitializeImage()
            {

                var streamRef = RandomAccessStreamReference.CreateFromUri(new Uri(Uri));
                using (var stream = await streamRef.OpenReadAsync())
                {
                    DispatcherHelpers.RunOnDispatcher(async () => await Image.SetSourceAsync(stream));
                }
            }
        }
    }
}
