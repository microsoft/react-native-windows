using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Annotations;
using System;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Effects;
using System.Windows.Media.Media3D;

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
        /// <summary>
        /// Set's the  <typeparamref name="TFrameworkElement"/> styling layout 
        /// properties, based on the <see cref="JObject"/> map.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="transforms">The list of transforms.</param>
        [ReactProp("transform")]
        public void SetTransform(TFrameworkElement view, JArray transforms)
        {
            if (transforms == null)
            {
                ResetProjectionMatrix(view);
            }
            else
            {
                SetProjectionMatrix(view, transforms);
            }
        }

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

        private static void SetProjectionMatrix(TFrameworkElement view, JArray transforms)
        {
            var transformMatrix = TransformHelper.ProcessTransform(transforms);

            var translateMatrix = Matrix3D.Identity;
            var translateBackMatrix = Matrix3D.Identity;
            if (!double.IsNaN(view.Width))
            {
                translateMatrix.OffsetX = -view.Width / 2;
                translateBackMatrix.OffsetX = view.Width / 2;
            }

            if (!double.IsNaN(view.Height))
            {
                translateMatrix.OffsetY = -view.Height / 2;
                translateBackMatrix.OffsetY = view.Height / 2;
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
