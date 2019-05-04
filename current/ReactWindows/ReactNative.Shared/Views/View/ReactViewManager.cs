// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
#if WINDOWS_UWP
using ReactNative.Accessibility;
using ReactNative.Reflection;
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
    public class ReactViewManager : ViewParentManager<BorderedCanvas>
    {
        private readonly ViewKeyedDictionary<BorderedCanvas, CornerRadiusManager> _borderedCanvasToRadii =
            new ViewKeyedDictionary<BorderedCanvas, CornerRadiusManager>();

        private readonly ViewKeyedDictionary<BorderedCanvas, ThicknessManager> _borderedCanvasToThickness =
            new ViewKeyedDictionary<BorderedCanvas, ThicknessManager>();

        /// <summary>
        /// The name of this view manager. This will be the name used to
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name => ViewProps.ViewClassName;

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override BorderedCanvas CreateViewInstance(ThemedReactContext reactContext)
        {
            return new BorderedCanvas();
        }

        /// <summary>
        /// Sets whether the view is collapsible.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="collapsible">The flag.</param>
        [ReactProp(ViewProps.Collapsible)]
        public void SetCollapsible(BorderedCanvas view, bool collapsible)
        {
            // no-op: it's here only so that "collapsable" prop is exported to JS. The value is actually
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

#if WINDOWS_UWP
        /// <summary>
        /// Set accessibility traits for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="accessibilityTraitsValue">Can be <see cref="JArray"/> of objects or a single object.
        ///     String representation of the object(s) is parsed as <see cref="AccessibilityTrait"/>.</param>
        [ReactProp(ViewProps.AccessibilityTraits)]
        public void SetAccessibilityTraits(BorderedCanvas view, object accessibilityTraitsValue)
        {
            AccessibilityHelper.SetAccessibilityTraits(view, accessibilityTraitsValue);
        }

        /// <summary>
        /// Sets <see cref="ImportantForAccessibility"/> for the BorderedCanvas.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="importantForAccessibilityValue">The string to be parsed as <see cref="ImportantForAccessibility"/>.</param>
        [ReactProp(ViewProps.ImportantForAccessibility)]
        public void SetImportantForAccessibility(BorderedCanvas view, string importantForAccessibilityValue)
        {
            var importantForAccessibility = EnumHelpers.ParseNullable<ImportantForAccessibility>(importantForAccessibilityValue)
                ?? ImportantForAccessibility.Auto;
            AccessibilityHelper.SetImportantForAccessibility(view, importantForAccessibility);
        }
#endif

        /// <summary>
        /// Enum values correspond to positions of prop names in ReactPropGroup attribute
        /// applied to <see cref="SetBorderRadius(BorderedCanvas, int, double?)"/>
        /// </summary>
        private enum Radius
        {
            All,
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
        }

        /// <summary>
        /// Sets the border radius of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="index">The prop index.</param>
        /// <param name="radius">The border radius value.</param>
        [ReactPropGroup(
            ViewProps.BorderRadius,
            ViewProps.BorderTopLeftRadius,
            ViewProps.BorderTopRightRadius,
            ViewProps.BorderBottomLeftRadius,
            ViewProps.BorderBottomRightRadius)]
        public void SetBorderRadius(BorderedCanvas view, int index, double? radius)
        {
            if (!_borderedCanvasToRadii.TryGetValue(view, out var cornerRadiusManager))
            {
                cornerRadiusManager = new CornerRadiusManager();
                _borderedCanvasToRadii.AddOrUpdate(view, cornerRadiusManager);
            }

            switch ((Radius)index)
            {
                case Radius.All:
                    cornerRadiusManager.Set(CornerRadiusManager.All, radius);
                    break;
                case Radius.TopLeft:
                    cornerRadiusManager.Set(CornerRadiusManager.TopLeft, radius);
                    break;
                case Radius.TopRight:
                    cornerRadiusManager.Set(CornerRadiusManager.TopRight, radius);
                    break;
                case Radius.BottomLeft:
                    cornerRadiusManager.Set(CornerRadiusManager.BottomLeft, radius);
                    break;
                case Radius.BottomRight:
                    cornerRadiusManager.Set(CornerRadiusManager.BottomRight, radius);
                    break;
            }
            view.CornerRadius = cornerRadiusManager.AsCornerRadius();
        }

        /// <summary>
        /// Sets the background color of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(
            ViewProps.BackgroundColor,
            CustomType = "Color")]
        public void SetBackgroundColor(BorderedCanvas view, uint? color)
        {
            view.Background = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Set the border color of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="color">The color hex code.</param>
        [ReactProp(ViewProps.BorderColor, CustomType = "Color")]
        public void SetBorderColor(BorderedCanvas view, uint? color)
        {
            view.BorderBrush = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Enum values correspond to positions of prop names in ReactPropGroup attribute
        /// applied to <see cref="SetBorderWidth(BorderedCanvas, int, double?)"/>
        /// </summary>
        private enum Width
        {
            All,
            Left,
            Right,
            Top,
            Bottom,
        }

        /// <summary>
        /// Sets the border thickness of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="index">The prop index.</param>
        /// <param name="width">The border width in pixels.</param>
        [ReactPropGroup(
            ViewProps.BorderWidth,
            ViewProps.BorderLeftWidth,
            ViewProps.BorderRightWidth,
            ViewProps.BorderTopWidth,
            ViewProps.BorderBottomWidth)]
        public void SetBorderWidth(BorderedCanvas view, int index, double? width)
        {
            if (!_borderedCanvasToThickness.TryGetValue(view, out var thicknessManager))
            {
                thicknessManager = new ThicknessManager();
                _borderedCanvasToThickness.AddOrUpdate(view, thicknessManager);
            }

            switch ((Width)index)
            {
                case Width.All:
                    thicknessManager.Set(ThicknessManager.All, width);
                    break;
                case Width.Left:
                    thicknessManager.Set(ThicknessManager.Left, width);
                    break;
                case Width.Right:
                    thicknessManager.Set(ThicknessManager.Right, width);
                    break;
                case Width.Top:
                    thicknessManager.Set(ThicknessManager.Top, width);
                    break;
                case Width.Bottom:
                    thicknessManager.Set(ThicknessManager.Bottom, width);
                    break;
            }
            view.BorderThickness = thicknessManager.AsThickness();
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, BorderedCanvas view)
        {
            base.OnDropViewInstance(reactContext, view);

            _borderedCanvasToRadii.Remove(view);
            _borderedCanvasToThickness.Remove(view);
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(BorderedCanvas parent, DependencyObject child, int index)
        {
            parent.Children.Insert(index, child.As<UIElement>());
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public override DependencyObject GetChildAt(BorderedCanvas parent, int index)
        {
            return parent.Children[index];
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(BorderedCanvas parent)
        {
            return parent.Children.Count;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(BorderedCanvas parent)
        {
            parent.Children.Clear();
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(BorderedCanvas parent, int index)
        {
            parent.Children.RemoveAt(index);
        }
    }
}
