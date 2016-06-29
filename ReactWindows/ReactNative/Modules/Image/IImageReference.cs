using System;
using System.Reactive;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Modules.Image
{
    interface IImageReference : IDisposable
    {
        BitmapImage Image { get; }

        IObservable<Unit> LoadedObservable { get; }
    }
}
