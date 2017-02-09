using System;
using System.Collections.Generic;
using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
#if WINDOWS_UWP
using Windows.UI.Composition;
using Windows.UI.Xaml.Controls;
#else
using System.Windows.Controls;
using System.Windows.Media.Effects;
#endif

namespace ReactNative.Views.View
{
    /// <summary>
    /// View manager for React view instances.
    /// </summary>
    public class ReactViewManager : BorderedCanvasManager<Canvas>
    {
        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTView";
            }
        }

        /// <summary>
        /// Sets whether or not the view is an accessibility element.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="accessible">A flag indicating whether or not the view is an accessibility element.</param>
        [ReactProp("accessible")]
        public void SetAccessible(Border view, bool accessible)
        {
            // TODO: #557 Provide implementation for View's accessible prop

            // We need to have this stub for this prop so that Views which
            // specify the accessible prop aren't considered to be layout-only.
            // The proper implementation is still to be determined.
        }

        /// <summary>
        /// Set the pointer events handling mode for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="pointerEventsValue">The pointerEvents mode.</param>
        [ReactProp(ViewProps.PointerEvents)]
        public void SetPointerEvents(Border view, string pointerEventsValue)
        {
            var pointerEvents = EnumHelpers.ParseNullable<PointerEvents>(pointerEventsValue) ?? PointerEvents.Auto;
            view.SetPointerEvents(pointerEvents);
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Canvas CreateInnerElement(ThemedReactContext reactContext)
        {
            return new Canvas();
        }

        [ReactProp(ViewProps.ShadowColor, CustomType = "Color")]
        public void SetShadowColor(Border view, uint? color)
        {
#if WINDOWS_UWP
            // TODO
#else
            DropShadowEffect effect = (DropShadowEffect) view.Effect ?? new DropShadowEffect();
            effect.Color = ColorHelpers.Parse(color.Value);
            view.Effect = effect;
#endif
        }

        [ReactProp(ViewProps.ShadowOffset)]
        public void SetShadowOffset(Border view, JObject offset)
        {
#if WINDOWS_UWP

#else
            DropShadowEffect effect = (DropShadowEffect) view.Effect ?? new DropShadowEffect();
            var deltaX = offset.Value<double>("width");
            var deltaY = offset.Value<double>("height");
            var angle = Math.Atan2(deltaY, deltaX) * (180 / Math.PI);
            var distance = Math.Sqrt(Math.Pow(deltaX, 2) + Math.Pow(deltaY, 2));
            effect.Direction = angle;
            effect.ShadowDepth = distance;
            view.Effect = effect;
#endif
        }

        [ReactProp(ViewProps.ShadowOpacity)]
        public void SetShadowOpacity(Border view, double opacity)
        {
#if WINDOWS_UWP

#else
            DropShadowEffect effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.Opacity = opacity;
            view.Effect = effect;
#endif
        }

        [ReactProp(ViewProps.ShadowRadius)]
        public void SetShadowRadius(Border view, double radius)
        {
#if WINDOWS_UWP

#else
            DropShadowEffect effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.BlurRadius = radius;
            view.Effect = effect;
#endif
        }
    }
}
