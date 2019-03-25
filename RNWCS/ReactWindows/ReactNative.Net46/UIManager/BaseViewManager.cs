// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.Touch;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Effects;
using System.Windows.Media.Media3D;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Base class that should be suitable for the majority of subclasses of <see cref="IViewManager"/>.
    /// It provides support for base view props such as opacity, etc.
    /// </summary>
    /// <typeparam name="TFrameworkElement">Type of framework element.</typeparam>
    /// <typeparam name="TLayoutShadowNode">Type of shadow node.</typeparam>
    public abstract class BaseViewManager<TFrameworkElement, TLayoutShadowNode> :
            ViewManager<TFrameworkElement, TLayoutShadowNode>
        where TFrameworkElement : FrameworkElement
        where TLayoutShadowNode : LayoutShadowNode
    {
        private readonly ViewKeyedDictionary<TFrameworkElement, Action<TFrameworkElement, Dimensions>> _transforms =
            new ViewKeyedDictionary<TFrameworkElement, Action<TFrameworkElement, Dimensions>>();

        /// <summary>
        /// Sets the 3D tranform on the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="transforms">
        /// The transform matrix or the list of transforms.
        /// </param>
        [ReactProp("transform")]
        public void SetTransform(TFrameworkElement view, JArray transforms)
        {
            if (transforms == null)
            {
                if (_transforms.Remove(view))
                {
                    ResetProjectionMatrix(view);
                }
            }
            else
            {
                _transforms[view] = (v, d) => SetProjectionMatrix(v, d, transforms);
                var dimensions = GetDimensions(view);
                SetProjectionMatrix(view, dimensions, transforms);
            }
        }

        /// <summary>
        /// Sets the opacity of the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="opacity">The opacity value.</param>
        [ReactProp(ViewProps.Opacity, DefaultDouble = 1.0)]
        public void SetOpacity(TFrameworkElement view, double opacity)
        {
            view.Opacity = opacity;
        }

        /// <summary>
        /// Sets the overflow prop for the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="overflow">The overflow value.</param>
        [ReactProp(ViewProps.Overflow)]
        public void SetOverflow(TFrameworkElement view, string overflow)
        {
            if (overflow == ViewProps.Hidden)
            {
                view.ClipToBounds = true;
            }
            else
            {
                view.ClipToBounds = false;
            }
        }

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
        /// Sets the display mode of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="display">The display mode.</param>
        [ReactProp(ViewProps.Display)]
        public void SetDisplay(TFrameworkElement view, string display)
        {
            view.Visibility = display == "none" ? Visibility.Collapsed : Visibility.Visible;
        }

        /// <summary>
        /// Sets the accessibility label of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="label">The label.</param>
        [ReactProp(ViewProps.AccessibilityLabel)]
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

        /// <summary>
        /// Sets a tooltip for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="tooltip">String to display in the tooltip.</param>
        [ReactProp("tooltip")]
        public void SetTooltip(TFrameworkElement view, string tooltip)
        {
            ToolTipService.SetToolTip(view, tooltip);
        }

        /// <summary>
        /// Set the pointer events handling mode for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="pointerEventsValue">The pointerEvents mode.</param>
        [ReactProp(ViewProps.PointerEvents)]
        public void SetPointerEvents(TFrameworkElement view, string pointerEventsValue)
        {
            var pointerEvents = EnumHelpers.ParseNullable<PointerEvents>(pointerEventsValue) ?? PointerEvents.Auto;
            view.SetPointerEvents(pointerEvents);
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup by the <see cref="IViewManager"/> subclass.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        /// <remarks>
        /// Be sure to call this base class method to register for pointer
        /// entered and pointer exited events.
        /// </remarks>
        public override void OnDropViewInstance(ThemedReactContext reactContext, TFrameworkElement view)
        {
            view.MouseEnter -= OnPointerEntered;
            view.MouseLeave -= OnPointerExited;
            _transforms.Remove(view);
            base.OnDropViewInstance(reactContext, view);
        }

        /// <summary>
        /// Subclasses can override this method to install custom event
        /// emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        /// <remarks>
        /// Consider overriding this method if your view needs to emit events
        /// besides basic touch events to JavaScript (e.g., scroll events).
        ///
        /// Make sure you call the base implementation to ensure base pointer
        /// event handlers are subscribed.
        /// </remarks>
        protected override void AddEventEmitters(ThemedReactContext reactContext, TFrameworkElement view)
        {
            view.MouseEnter += OnPointerEntered;
            view.MouseLeave += OnPointerExited;
        }

        private void OnPointerEntered(object sender, MouseEventArgs e)
        {
            var view = (TFrameworkElement)sender;
            TouchHandler.OnPointerEntered(view, e);
        }

        private void OnPointerExited(object sender, MouseEventArgs e)
        {
            var view = (TFrameworkElement)sender;
            TouchHandler.OnPointerExited(view, e);
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The dimensions.</param>
        public override void SetDimensions(TFrameworkElement view, Dimensions dimensions)
        {
            Action<TFrameworkElement, Dimensions> applyTransform;
            if (_transforms.TryGetValue(view, out applyTransform))
            {
                applyTransform(view, dimensions);
            }
            base.SetDimensions(view, dimensions);
        }

        /// <summary>
        /// Sets the shadow color of the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The shadow color.</param>
        [ReactProp("shadowColor", CustomType = "Color")]
        public void SetShadowColor(TFrameworkElement view, uint? color)
        {
            var effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.Color = ColorHelpers.Parse(color.Value);
            view.Effect = effect;
        }

        /// <summary>
        /// Sets the shadow offset of the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="offset">The shadow offset.</param>
        [ReactProp("shadowOffset")]
        public void SetShadowOffset(TFrameworkElement view, JObject offset)
        {
            var effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            var deltaX = offset.Value<double>("width");
            var deltaY = offset.Value<double>("height");
            var angle = Math.Atan2(deltaY, deltaX) * (180 / Math.PI);
            var distance = Math.Sqrt(Math.Pow(deltaX, 2) + Math.Pow(deltaY, 2));
            effect.Direction = angle;
            effect.ShadowDepth = distance;
            view.Effect = effect;
        }

        /// <summary>
        /// Sets the shadow opacity of the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="opacity">The shadow opacity.</param>
        [ReactProp("shadowOpacity")]
        public void SetShadowOpacity(TFrameworkElement view, double opacity)
        {
            var effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.Opacity = opacity;
            view.Effect = effect;
        }

        /// <summary>
        /// Sets the shadow radius of the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="radius">The shadow radius.</param>
        [ReactProp("shadowRadius")]
        public void SetShadowRadius(TFrameworkElement view, double radius)
        {
            var effect = (DropShadowEffect)view.Effect ?? new DropShadowEffect();
            effect.BlurRadius = radius;
            view.Effect = effect;
        }

        private static void SetProjectionMatrix(TFrameworkElement view, Dimensions dimensions, JArray transforms)
        {
            var transformMatrix = TransformHelper.ProcessTransform(transforms);

            var translateMatrix = Matrix3D.Identity;
            var translateBackMatrix = Matrix3D.Identity;
            if (!double.IsNaN(dimensions.Width))
            {
                translateMatrix.OffsetX = -dimensions.Width / 2;
                translateBackMatrix.OffsetX = dimensions.Width / 2;
            }

            if (!double.IsNaN(dimensions.Height))
            {
                translateMatrix.OffsetY = -dimensions.Height / 2;
                translateBackMatrix.OffsetY = dimensions.Height / 2;
            }

            var projectionMatrix = translateMatrix * transformMatrix * translateBackMatrix;
            ApplyProjection(view, projectionMatrix);
        }

        private static void ApplyProjection(TFrameworkElement view, Matrix3D projectionMatrix)
        {
            if (!projectionMatrix.IsAffine)
            {
                throw new NotImplementedException("ReactNative.Net46 does not support non-affine transformations");
            }

            if (IsSimpleTranslationOnly(projectionMatrix))
            {
                ResetProjectionMatrix(view);
                var transform = new MatrixTransform();
                var matrix = transform.Matrix;
                matrix.OffsetX = projectionMatrix.OffsetX;
                matrix.OffsetY = projectionMatrix.OffsetY;
                transform.Matrix = matrix;
                view.RenderTransform = transform;
            }
            else
            {
                var transform = new MatrixTransform(projectionMatrix.M11,
					projectionMatrix.M12,
                    projectionMatrix.M21,
                    projectionMatrix.M22,
                    projectionMatrix.OffsetX,
                    projectionMatrix.OffsetY);

                view.RenderTransform = transform;
            }
        }

        private static bool IsSimpleTranslationOnly(Matrix3D matrix)
        {
            // Matrix3D is a struct and passed-by-value. As such, we can modify
            // the values in the matrix without affecting the caller.
            matrix.OffsetX = matrix.OffsetY = 0;
            return matrix.IsIdentity;
        }

        private static void ResetProjectionMatrix(TFrameworkElement view)
        {
            var transform = view.RenderTransform;
            var matrixTransform = transform as MatrixTransform;
            if (transform != null && matrixTransform == null)
            {
                throw new InvalidOperationException("Unknown projection set on framework element.");
            }

            view.RenderTransform = null;
        }
    }
}
