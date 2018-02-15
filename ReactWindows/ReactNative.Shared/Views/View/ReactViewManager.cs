using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System.Runtime.CompilerServices;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Linq;
#if WINDOWS_UWP
using ReactNative.Common;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Data;
#endif

namespace ReactNative.Views.View
{
    /// <summary>
    /// View manager for React view instances.
    /// </summary>
    public class ReactViewManager : ViewParentManager<BorderedCanvas>
    {
        private enum Radius
        {
            All,
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
        }

        private class BorderProps
        {
            public uint? Color;
        }

        private readonly ConditionalWeakTable<BorderedCanvas, BorderProps> _borderProps =
            new ConditionalWeakTable<BorderedCanvas, BorderProps>();

        /// <summary>
        /// Some border props do not affect appearance unless they are combined
        /// with other props. Such props are saved in the dictionary and applied
        /// later. This allows to not create the inner Border if it's not visible.
        /// </summary>
        private BorderProps GetBorderProps(BorderedCanvas view)
        {
            BorderProps props;

            if (!_borderProps.TryGetValue(view, out props))
            {
                props = new BorderProps();
                _borderProps.Add(view, props);
            }

            return props;
        }

        /// <summary>
        /// Canvas.Background supports flat backgrounds. Border.Background supports
        /// backgrounds with customizations, such as rounded corners. If the background
        /// is flat, it's set on Canvas. If it has cutomizations, it's transferred to Border.
        /// </summary>
        private void TransferBackgroundBrush(BorderedCanvas view)
        {
            if (view.Background != null && view.Background != _defaultBackgroundBrush && view.Border != null)
            {
                view.Border.Background = view.Background;
                view.Background = _defaultBackgroundBrush;
            }
        }

        /// <summary>
        /// Default brush for the view borders.
        /// </summary>
        protected readonly Brush _defaultBorderBrush = new SolidColorBrush(Colors.Black);

        /// <summary>
        /// In WPF in order to be clickable (hit-test visible) the element needs to have background brush.
        /// This is why when the background and border brushes are set on the inner Border, the Canvas gets
        /// a transparent background brush.
        /// </summary>                
        protected readonly Brush _defaultBackgroundBrush
#if WINDOWS_UWP
            = null;
#else
            = new SolidColorBrush(Color.FromArgb(0, 0, 0, 0));
#endif

        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name
        {
            get
            {
                return ViewProps.ViewClassName;
            }
        }

        /// <summary>
        /// Checks if the Canvas has a Border already.
        /// </summary>
        protected bool HasBorder(BorderedCanvas view)
        {
            return view.Border != null;
        }

        /// <summary>
        /// Adds a Border to a Canvas if it hasn't been added already.
        /// </summary>
        protected Border GetOrCreateBorder(BorderedCanvas view)
        {
            if (view.Border == null)
            {
                view.Border = new Border { BorderBrush = _defaultBorderBrush };

                // Layout animations bypass SetDimensions, hence using XAML bindings.

                view.Border.SetBinding(FrameworkElement.WidthProperty, new Binding
                {
                    Source = view,
                    Path = new PropertyPath("Width")
                });

                view.Border.SetBinding(FrameworkElement.HeightProperty, new Binding
                {
                    Source = view,
                    Path = new PropertyPath("Height")
                });
            }

            return view.Border;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override BorderedCanvas CreateViewInstance(ThemedReactContext reactContext)
        {
            var borderedCanvas = new BorderedCanvas();

#if WINDOWS_UWP
            // Setting AutomationProperties.Name to some string and then clearing it will guarantee that
            // AutomationPeer is always created for the canvas. The default implementation does not
            // create AutomationPeer for canvas if AutomationProperties.Name has never been set,
            // but to implement accessibility it is required that the AutomationPeer is always created.
            AutomationProperties.SetName(borderedCanvas, " ");
            borderedCanvas.ClearValue(AutomationProperties.NameProperty);
#endif

            return borderedCanvas;
        }

        #region misc RN props

        /// <summary>
        /// Sets whether the view is collapsible.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="collapsible">The flag.</param>
        [ReactProp(ViewProps.Collapsible)]
        public void SetCollapsible(BorderedCanvas view, bool collapsible)
        {
            // no-op: it's here only so that "collapsable" property is exported to JS. The value is actually
            // handled in NativeViewHierarchyOptimizer
        }

        /// <summary>
        /// Sets whether or not the view is an accessibility element.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="accessible">A flag indicating whether or not the view is an accessibility element.</param>
        [ReactProp("accessible")]
        public void SetAccessible(BorderedCanvas view, bool accessible)
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
        [ReactProp("pointerEvents")]
        public void SetPointerEvents(BorderedCanvas view, string pointerEventsValue)
        {
            var pointerEvents = EnumHelpers.ParseNullable<PointerEvents>(pointerEventsValue) ?? PointerEvents.Auto;
            view.SetPointerEvents(pointerEvents);
        }

        /// <summary>
        /// Set accessibility traits for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="accessibilityTraitsValue">Can be <see cref="JArray"/> of objects or a single object. 
        ///     String representation of the object(s) is parsed as <see cref="AccessibilityTrait"/>.</param>
        [ReactProp("accessibilityTraits")]
        public void SetAccessibilityTraits(BorderedCanvas view, object accessibilityTraitsValue)
        {
            AccessibilityTrait[] result = null;
            if (accessibilityTraitsValue != null)
            {
                if (accessibilityTraitsValue is JArray asJArray)
                {
                    var list = new List<AccessibilityTrait>();
                    foreach (var s in asJArray.Values<string>())
                    {
                        if (EnumHelpers.TryParse<AccessibilityTrait>(s, out var accessibilityTrait))
                        {
                            list.Add(accessibilityTrait);
                        }
                    }
                    result = list.Any() ? list.ToArray() : null;
                }
                else
                {
                    if (EnumHelpers.TryParse<AccessibilityTrait>(accessibilityTraitsValue.ToString(), out var accessibilityTrait))
                    {
                        result = new[] { accessibilityTrait };
                    }
                }
            }
            view.AccessibilityTraits = result;
        }

        /// <summary>
        /// Sets <see cref="ImportantForAccessibility"/> for the BorderedCanvas.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="importantForAccessibilityValue">The string to be parsed as <see cref="ImportantForAccessibility"/>.</param>
        [ReactProp("importantForAccessibility")]
        public void SetImportantForAccessibility(BorderedCanvas view, string importantForAccessibilityValue)
        {
#if WINDOWS_UWP
            var importantForAccessibility = EnumHelpers.ParseNullable<ImportantForAccessibility>(importantForAccessibilityValue) ?? ImportantForAccessibility.Auto;
            AccessibilityHelper.SetImportantForAccessibility(view, importantForAccessibility);
#endif
        }

        #endregion

        #region borders and background

        /// <summary>
        /// Sets the border radius of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="index">The property index.</param>
        /// <param name="radius">The border radius value.</param>
        [ReactPropGroup(
            ViewProps.BorderRadius,
            ViewProps.BorderTopLeftRadius,
            ViewProps.BorderTopRightRadius,
            ViewProps.BorderBottomLeftRadius,
            ViewProps.BorderBottomRightRadius)]
        public void SetBorderRadius(BorderedCanvas view, int index, double radius)
        {
            var border = GetOrCreateBorder(view);
            var cornerRadius = border.CornerRadius == null ? new CornerRadius() : border.CornerRadius;

            switch ((Radius)index)
            {
                case Radius.All:
                    cornerRadius = new CornerRadius(radius);
                    break;
                case Radius.TopLeft:
                    cornerRadius.TopLeft = radius;
                    break;
                case Radius.TopRight:
                    cornerRadius.TopRight = radius;
                    break;
                case Radius.BottomLeft:
                    cornerRadius.BottomLeft = radius;
                    break;
                case Radius.BottomRight:
                    cornerRadius.BottomRight = radius;
                    break;
            }

            border.CornerRadius = cornerRadius;
            TransferBackgroundBrush(view);
        }

        /// <summary>
        /// Sets the background color of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(
            ViewProps.BackgroundColor,
            CustomType = "Color",
            DefaultUInt32 = ColorHelpers.Transparent)]
        public void SetBackgroundColor(BorderedCanvas view, uint color)
        {
            var brush = new SolidColorBrush(ColorHelpers.Parse(color));

            if (view.Border == null)
            {
                view.Background = brush;
            }
            else
            {
                var border = GetOrCreateBorder(view);
                border.Background = brush;
                view.Background = view.Background ?? _defaultBackgroundBrush;
            }
        }

        /// <summary>
        /// Set the border color of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="color">The color hex code.</param>
        [ReactProp("borderColor", CustomType = "Color")]
        public void SetBorderColor(BorderedCanvas view, uint? color)
        {
            if (view.Border == null)
            {
                GetBorderProps(view).Color = color;
            }
            else
            {
                var border = GetOrCreateBorder(view);
                border.BorderBrush = color.HasValue
                    ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                    : _defaultBorderBrush;
                TransferBackgroundBrush(view);
            }
        }

        /// <summary>
        /// Sets the border thickness of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="index">The property index.</param>
        /// <param name="width">The border width in pixels.</param>
        [ReactPropGroup(
            ViewProps.BorderWidth,
            ViewProps.BorderLeftWidth,
            ViewProps.BorderRightWidth,
            ViewProps.BorderTopWidth,
            ViewProps.BorderBottomWidth)]
        public void SetBorderWidth(BorderedCanvas view, int index, double width)
        {
            var border = GetOrCreateBorder(view);
            border.SetBorderWidth(ViewProps.BorderSpacingTypes[index], width);

            if (border.BorderBrush == null)
            {
                var color = GetBorderProps(view).Color;
                SetBorderColor(view, color);
            }

            TransferBackgroundBrush(view);
        }

        #endregion

        #region children collection management

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(BorderedCanvas parent, DependencyObject child, int index)
        {
            if (HasBorder(parent))
            {
                index++;
            }

            var uiElementChild = child.As<UIElement>();
            parent.Children.Insert(index, uiElementChild);

#if WINDOWS_UWP
            AccessibilityHelper.OnChildAdded(parent, uiElementChild);
#endif
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public override DependencyObject GetChildAt(BorderedCanvas parent, int index)
        {
            if (HasBorder(parent))
            {
                index++;
            }

            return parent.Children[index];
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(BorderedCanvas parent)
        {
            var count = parent.Children.Count;

            if (HasBorder(parent))
            {
                count--;
            }

            return count;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(BorderedCanvas parent)
        {
            if (HasBorder(parent))
            {
                for (var i = parent.Children.Count - 1; i > 0; i--)
                {
                    parent.Children.RemoveAt(i);
                }
            }
            else
            {
                parent.Children.Clear();
            }

#if WINDOWS_UWP
            AccessibilityHelper.OnChildRemoved(parent);
#endif
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(BorderedCanvas parent, int index)
        {
            if (HasBorder(parent))
            {
                index++;
            }

            parent.Children.RemoveAt(index);

#if WINDOWS_UWP
            AccessibilityHelper.OnChildRemoved(parent);
#endif
        }

        #endregion
    }
}
