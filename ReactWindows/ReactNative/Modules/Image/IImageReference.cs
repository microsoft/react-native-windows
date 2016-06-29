using System;
using System.Reactive;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Modules.Image
{
    /// <summary>
    /// A reference to an image.
    /// </summary>
    public interface IImageReference : IDisposable
    {
        /// <summary>
        /// The image.
        /// </summary>
        BitmapImage Image { get; }

        /// <summary>
        /// An observable sequence for when the image is loaded.
        /// </summary>
        /// <remarks>
        /// The sequence will send an error event if the load fails.
        /// </remarks>
        IObservable<Unit> LoadedObservable { get; }
    }
}
