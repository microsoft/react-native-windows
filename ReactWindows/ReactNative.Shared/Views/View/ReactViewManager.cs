using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
#if WINDOWS_UWP
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
#endif

namespace ReactNative.Views.View
{
    /// <summary>
    /// View manager for React view instances.
    /// </summary>
    public class ReactViewManager : ViewParentManager<Border>
    {
        private enum Radius
        {
            All,
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
        }

        private static readonly Brush s_defaultBorderBrush = new SolidColorBrush(Colors.Black);

        private readonly BorderCanvasViewPool _poolInstance = new BorderCanvasViewPool(256);

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

        public override void AddView(Border parent, DependencyObject child, int index)
        {
            var canvas = (Canvas)parent.Child;
            var uiEement = (UIElement)child;
            canvas.Children.Insert(index, uiEement);
        }

        public override DependencyObject GetChildAt(Border parent, int index)
        {
            var canvas = (Canvas)parent.Child;
            return canvas.Children[index];
        }

        public override int GetChildCount(Border parent)
        {
            var canvas = (Canvas)parent.Child;
            return canvas.Children.Count;
        }

        public override void RemoveAllChildren(Border parent)
        {
            var canvas = (Canvas)parent.Child;
            canvas.Children.Clear();
        }

        public override void RemoveChildAt(Border parent, int index)
        {
            var canvas = (Canvas)parent.Child;
            canvas.Children.RemoveAt(index);
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
        [ReactProp("pointerEvents")]
        public void SetPointerEvents(Border view, string pointerEventsValue)
        {
            var pointerEvents = EnumHelpers.ParseNullable<PointerEvents>(pointerEventsValue) ?? PointerEvents.Auto;
            view.SetPointerEvents(pointerEvents);
        }


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
        public void SetBorderRadius(Border view, int index, double radius)
        {
            var cornerRadius = view.CornerRadius == null ? new CornerRadius() : view.CornerRadius;

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

            view.CornerRadius = cornerRadius;
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
        public void SetBackgroundColor(Border view, uint color)
        {
            view.Background = new SolidColorBrush(ColorHelpers.Parse(color));
        }

        /// <summary>
        /// Set the border color of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="color">The color hex code.</param>
        [ReactProp("borderColor", CustomType = "Color")]
        public void SetBorderColor(Border view, uint? color)
        {
            view.BorderBrush = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : s_defaultBorderBrush;
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
        public void SetBorderWidth(Border view, int index, double width)
        {
            view.SetBorderWidth(ViewProps.BorderSpacingTypes[index], width);
        }

        /// <summary>
        /// Sets whether the view is collapsible.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="collapsible">The flag.</param>
        [ReactProp(ViewProps.Collapsible)]
        public void SetCollapsible(Border view, bool collapsible)
        {
            // no-op: it's here only so that "collapsable" property is exported to JS. The value is actually
            // handled in NativeViewHierarchyOptimizer
        }

        public override void OnDropViewInstance(ThemedReactContext reactContext, Border view)
        {
            base.OnDropViewInstance(reactContext, view);
            _poolInstance.Free(view);
        }


        protected override Border CreateViewInstance(ThemedReactContext reactContext)
        {
            return _poolInstance.Allocate();
        }

        private static Border CreateNewViewInstance()
        {
            return new Border
            {
                Child = new Canvas(),
            };
        }

        class BorderCanvasViewPool : BaseViewPool<Border>
        {
            public BorderCanvasViewPool() 
                : base(CreateNewViewInstance)
            {
            }

            public BorderCanvasViewPool(int size) 
                : base(CreateNewViewInstance, size)
            {
            }

            protected override void FreeCore(Border obj)
            {
                obj.SetPointerEvents(PointerEvents.Auto);
                obj.BorderThickness = default(Thickness);
                obj.CornerRadius = default(CornerRadius);
                obj.BorderBrush = s_defaultBorderBrush;
                obj.Background = new SolidColorBrush(Colors.Transparent);
                ((Canvas)obj.Child).Children.Clear();
                base.FreeCore(obj);
            }
        }
    }
}
