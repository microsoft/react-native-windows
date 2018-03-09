// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reactive;
using System.Reactive.Linq;
#if WINDOWS_UWP
using Windows.UI.Xaml.Media.Animation;
#else
using System.Windows.Media.Animation;
#endif

namespace ReactNative.UIManager.LayoutAnimation
{
    class StoryboardObservable : IObservable<Unit>
    {
        private readonly Storyboard _storyboard;
        private readonly Action _finally;

        public StoryboardObservable(Storyboard storyboard)
            : this(storyboard, null)
        {
        }

        public StoryboardObservable(Storyboard storyboard, Action @finally)
        {
            _storyboard = storyboard;
            _finally = @finally;
        }

        public IDisposable Subscribe(IObserver<Unit> observer)
        {
            _storyboard.Begin();

#if WINDOWS_UWP
            return Observable.FromEventPattern<object>(
                h => _storyboard.Completed += h,
                h => _storyboard.Completed -= h)
#else
            return Observable.FromEventPattern(
                h => _storyboard.Completed += h,
                h => _storyboard.Completed -= h)
#endif
                .Select(v => default(Unit))
                .Finally(() =>
                {
                    _storyboard.Stop();
                    if (_finally != null)
                    {
                        _finally();
                    }
                })
                .Take(1)
                .Subscribe(observer);
        }
    }
}
