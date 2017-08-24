using System;
using System.Linq;
using System.Numerics;
using Windows.Foundation.Metadata;
using Windows.Graphics.Effects;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Hosting;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// Manages all Windows.UI.Composition resources.
    /// </summary>
    public class EffectManager : IDisposable
    {
        /// <summary>
        /// Gets a value indicating whether Windows 10 SDK >= 14393, which supports
        /// <see cref="CompositionBackdropBrush"/>.
        /// </summary>
        public static readonly bool IsCompositionSupported = 
            ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 3);

        private Visual _rootVisual;
        private Compositor _compositor;
        private ContainerVisual _containerVisual;

        /// <summary>
        /// Initializes an instance of a <see cref="EffectManager"/>.
        /// </summary>
        /// <param name="element">A base element class for Windows Runtime UI objects.</param>
        public EffectManager(FrameworkElement element)
        {
            _rootVisual = ElementCompositionPreview.GetElementVisual(element);
            _compositor = _rootVisual.Compositor;
            _containerVisual = _compositor.CreateContainerVisual();
            ElementCompositionPreview.SetElementChildVisual(element, _containerVisual);

            // Resize the sprite visuals when element changes.
            element.SizeChanged += (sender, e) =>
            {
                foreach (var child in _containerVisual.Children)
                {
                    child.Size = new Vector2((float)e.NewSize.Width, (float)e.NewSize.Height);
                }
            };
        }

        /// <summary>
        /// Set the blur radius of the image view.
        /// </summary>
        /// <param name="blurRadius">The blur radius value.</param>
        public void SetBlurRadius(double blurRadius)
        {
            ApplyEffect(BlurEffect.Get((float)blurRadius), BlurEffect.Name);
        }

        /// <summary>
        /// Set the tint color of the image view.
        /// </summary>
        /// <param name="color">The tint color value.</param>
        public void SetTintColor(uint color)
        {
            // Change the composite mode of the root visual to DestinationInvert.
            // This is only needed for the tint color effect.
            _rootVisual.CompositeMode = CompositionCompositeMode.DestinationInvert;

            // Create the tint effect.
            ApplyEffect(TintColorEffect.Get(color), TintColorEffect.Name);
        }

        private SpriteVisual CreateSpriteVisual(string name, CompositionBrush brush)
        {
            var spriteVisual = _containerVisual.Children.FirstOrDefault(
                child => child.Comment.Equals(name)) as SpriteVisual;

            if (spriteVisual != null)
            {
                var oldBrush = spriteVisual.Brush;
                spriteVisual.Brush = brush;
                oldBrush.Dispose();
            }
            else
            {
                spriteVisual = _compositor.CreateSpriteVisual();
                spriteVisual.Brush = brush;
            }

            spriteVisual.Comment = name;
            return spriteVisual;
        }

        private void ApplyEffect(IGraphicsEffect effect, string name)
        {
            using (effect as IDisposable)
            using (var effectFactory = _compositor.CreateEffectFactory(effect, null))
            {
                // Create the effect brush
                var effectBrush = effectFactory.CreateBrush();
                var backdropBrush = _compositor.CreateBackdropBrush();
                effectBrush.SetSourceParameter("Source", backdropBrush);

                // Create the sprite visual and update its brush
                var spriteVisual = CreateSpriteVisual(name, effectBrush);

                // Insert the sprite visual to the container visual
                _containerVisual.Children.InsertAtTop(spriteVisual);
            }
        }

        /// <summary>
        /// Disposes the Windows.UI.Composition resources.
        /// </summary>
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                _containerVisual.Dispose();
            }
        }

        /// <summary>
        /// Disposes the Windows.UI.Composition resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
        }
    }
}
