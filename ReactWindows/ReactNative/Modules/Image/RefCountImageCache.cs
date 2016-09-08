using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reactive;
using System.Reactive.Concurrency;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Threading;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Modules.Image
{
    class RefCountImageCache : IImageCache
    {
        private readonly IDictionary<string, ImageReference> _cache =
            new Dictionary<string, ImageReference>();

        private readonly IUriLoader _uriLoader;
         
        public RefCountImageCache(IUriLoader uriLoader)
        {
            _uriLoader = uriLoader;
        }

        public IImageReference Get(string uri)
        {
            var reference = default(ImageReference);
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
                        reference = new ImageReference(uri, this);
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

        class ImageReference : IImageReference
        {
            private readonly RefCountImageCache _parent;
            private readonly ReplaySubject<Unit> _subject;
            private readonly ObserverWrapper _observerWrapper;
            private readonly SingleAssignmentDisposable _subscription;

            private int _refCount = 1;

            public ImageReference(string uri, RefCountImageCache parent)
            {
                Uri = uri;
                _parent = parent;
                _subject = new ReplaySubject<Unit>(1);
                _observerWrapper = new ObserverWrapper(_subject);
                _subscription = new SingleAssignmentDisposable();

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
                        _observerWrapper.Dispose();
                        _subject.Dispose();
                        _parent._cache.Remove(Uri);
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
                        throw new InvalidOperationException(pattern.EventArgs.ErrorMessage);
                    });

                _subscription.Disposable = openedObservable
                    .Merge(failedObservable)
                    .Subscribe(_observerWrapper);
            }

            private async void InitializeImage()
            {
                try
                {
                    var stream = await _parent._uriLoader.OpenReadAsync(Uri).ConfigureAwait(false);
                    DispatcherHelpers.RunOnDispatcher(async () =>
                    {
                        Image = new BitmapImage();
                        DoSubscribe(Image);
                        try
                        {
                            await Image.SetSourceAsync(stream).AsTask().ConfigureAwait(false);
                        }
                        finally
                        {
                            stream.Dispose();
                        }
                    });
                }
                catch (FileNotFoundException ex)
                {
                    _observerWrapper.OnError(ex);
                }
            }

            class ObserverWrapper : IObserver<Unit>, IDisposable
            {
                private static readonly IObserver<Unit> s_nop = Observer.Create<Unit>(_ => { });

                private readonly object _gate = new object();

                private IObserver<Unit> _observer;

                public ObserverWrapper(IObserver<Unit> inner)
                {
                    _observer = inner;
                }

                public void OnCompleted()
                {
                    lock (_gate)
                    {
                        _observer.OnCompleted();
                    }
                }

                public void OnError(Exception error)
                {
                    lock (_gate)
                    {
                        _observer.OnError(error);
                    }
                }

                public void OnNext(Unit value)
                {
                    lock (_gate)
                    {
                        _observer.OnNext(value);
                    }
                }

                public void Dispose()
                {
                    lock (_gate)
                    {
                        _observer = s_nop;
                    }
                }
            }
        }
    }
}
