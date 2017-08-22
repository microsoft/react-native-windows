using Microsoft.Graphics.Canvas.Effects;
using Windows.Graphics.Effects;
using Windows.UI.Composition;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// Creates the blur effect.
    /// </summary>
    public static class BlurEffect
    {
        /// <summary>
        /// The max blur radius on iOS/Android is 25 and Win2D is 3.
        /// </summary>
        private const float BlurAmountScale = 3.0f / 25;

        /// <summary>
        /// The name of the effect.
        /// </summary>
        public static string Name { get; } = "Blur";

        /// <summary>
        /// Gets the <see cref="IGraphicsEffect"/> of the blur.
        /// </summary>
        /// <param name="blurRadius">The blur radius value.</param>
        public static IGraphicsEffect Get(float blurRadius)
        {
            return new GaussianBlurEffect
            {
                Name = Name,
                BlurAmount = blurRadius * BlurAmountScale,
                Source = new CompositionEffectSourceParameter("Source"),
            };
        }
    }
}
