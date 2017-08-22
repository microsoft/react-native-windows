using Microsoft.Graphics.Canvas.Effects;
using ReactNative.UIManager;
using Windows.Graphics.Effects;
using Windows.UI.Composition;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// Creates the tint color effect.
    /// </summary>
    public static class TintColorEffect
    {
        /// <summary>
        /// The name of the effect.
        /// </summary>
        public static string Name { get; } = "TintColor";

        /// <summary>
        /// Gets the <see cref="IGraphicsEffect"/> of the tint color.
        /// </summary>
        /// <param name="color">The tint color value.</param>
        public static IGraphicsEffect Get(uint color)
        {
            return new ArithmeticCompositeEffect
            {
                Name = Name,
                Source1 = new ColorSourceEffect()
                {
                    Name = "Tint",
                    Color = ColorHelpers.Parse(color),
                },
                Source1Amount = 1,

                Source2 = new CompositionEffectSourceParameter("Source"),
                Source2Amount = -1,

                MultiplyAmount = 0,
                Offset = 0,
            };
        }
    }
}
