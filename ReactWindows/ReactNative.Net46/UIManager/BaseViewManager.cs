using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Annotations;
using System;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Media.Effects;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Base class that should be suitable for the majority of subclasses of <see cref="IViewManager"/>.
    /// It provides support for base view properties such as opacity, etc.
    /// </summary>
    /// <typeparam name="TFrameworkElement">Type of framework element.</typeparam>
    /// <typeparam name="TLayoutShadowNode">Type of shadow node.</typeparam>
    public abstract class BaseViewManager<TFrameworkElement, TLayoutShadowNode> :
            ViewManager<TFrameworkElement, TLayoutShadowNode>
        where TFrameworkElement : FrameworkElement
        where TLayoutShadowNode : LayoutShadowNode
    {
        // ToDo: SetTransform - ReactProp("transform")

        /// <summary>
        /// Sets the opacity of the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="opacity">The opacity value.</param>
        [ReactProp("opacity", DefaultDouble = 1.0)]
        public void SetOpacity(TFrameworkElement view, double opacity)
        {
            view.Opacity = opacity;
        }

        // ToDo: SetOverflow - ReactProp("overflow")

        /// <summary>
        /// Sets the z-index of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zIndex">The z-index.</param>
        [ReactProp("zIndex")]
        public void SetZIndex(TFrameworkElement view, int zIndex)
        {
            Canvas.SetZIndex(view, zIndex);
        }

        /// <summary>
        /// Sets the accessibility label of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="label">The label.</param>
        [ReactProp("accessibilityLabel")]
        public void SetAccessibilityLabel(TFrameworkElement view, string label)
        {
            AutomationProperties.SetName(view, label ?? "");
        }

        // ToDo: SetAccessibilityLiveRegion - ReactProp("accessibilityLiveRegion")

        /// <summary>
        /// Sets the test ID, i.e., the automation ID.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="testId">The test ID.</param>
        [ReactProp("testID")]
        public void SetTestId(TFrameworkElement view, string testId)
        {
            AutomationProperties.SetAutomationId(view, testId ?? "");
        }

        [ReactProp(ViewProps.ShadowColor, CustomType = "Color")]
        public void SetShadowColor(TFrameworkElement view, uint? color)
        {
            DropShadowEffect effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.Color = ColorHelpers.Parse(color.Value);
            view.Effect = effect;
        }

        [ReactProp(ViewProps.ShadowOffset)]
        public void SetShadowOffset(TFrameworkElement view, JObject offset)
        {
            DropShadowEffect effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            var deltaX = offset.Value<double>("width");
            var deltaY = offset.Value<double>("height");
            var angle = Math.Atan2(deltaY, deltaX) * (180 / Math.PI);
            var distance = Math.Sqrt(Math.Pow(deltaX, 2) + Math.Pow(deltaY, 2));
            effect.Direction = angle;
            effect.ShadowDepth = distance;
            view.Effect = effect;
        }

        [ReactProp(ViewProps.ShadowOpacity)]
        public void SetShadowOpacity(TFrameworkElement view, double opacity)
        {
            DropShadowEffect effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.Opacity = opacity;
            view.Effect = effect;
        }

        [ReactProp(ViewProps.ShadowRadius)]
        public void SetShadowRadius(TFrameworkElement view, double radius)
        {
            DropShadowEffect effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.BlurRadius = radius;
            view.Effect = effect;
        }
    }
}
