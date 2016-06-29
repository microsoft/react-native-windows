using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Reactive;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Threading;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Modules.Image
{
    class InMemoryImageCache : IImageCache
    {
        private readonly IDictionary<string, Impl> _cache =
            new Dictionary<string, Impl>();

        private readonly IUriLoader _uriLoader;
         
        public InMemoryImageCache(IUriLoader uriLoader)
        {
            _uriLoader = uriLoader;
        }

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
                        reference = new Impl(uri, this);
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
            private readonly ReplaySubject<Unit> _subject;

            private IDisposable _subscription;
            private int _refCount = 1;

            public Impl(string uri, InMemoryImageCache parent)
            {
                Uri = uri;
                _parent = parent;
                _subject = new ReplaySubject<Unit>(1);
                InitializeImage();
            }

            public string Uri
            {
                get;
            }

            public BitmapImage Image
            {
                get;
                private set;
            }

            public IObservable<Unit> LoadedObservable
            {
                get
                {
                    return _subject;
                }
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
                        _subscription.Dispose();
                        _subject.Dispose();
                        _parent.Release(this);
                    }
                }
            }

            private void DoSubscribe(BitmapImage image)
            {
                var openedObservable = Observable.FromEventPattern<RoutedEventHandler, RoutedEventArgs>(
                    h => image.ImageOpened += h,
                    h => image.ImageOpened -= h)
                    .Select(_ => default(Unit));

                var failedObservable = Observable.FromEventPattern<ExceptionRoutedEventHandler, ExceptionRoutedEventArgs>(
                    h => image.ImageFailed += h,
                    h => image.ImageFailed -= h)
                    .Select<EventPattern<ExceptionRoutedEventArgs>, Unit>(pattern =>
                    {
                        throw new Exception(pattern.EventArgs.ErrorMessage);
                    });

                _subscription = openedObservable
                    .Merge(failedObservable)
                    .Subscribe(_subject);
            }

            private async void InitializeImage()
            {
                var stream = await _parent._uriLoader.OpenReadAsync(Uri);
                DispatcherHelpers.RunOnDispatcher(async () =>
                {
                    Image = new BitmapImage();
                    DoSubscribe(Image);
                    try
                    {
                        await Image.SetSourceAsync(stream);
                    }
                    finally
                    {
                        stream.Dispose();
                    }
                });
            }
        }
    }
}
