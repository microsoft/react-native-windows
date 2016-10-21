using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Annotations;
using System;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Media3D;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Windows.Automation;
#endif

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

        /// <summary>
        /// Sets the overflow property for the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="overflow">The overflow value.</param>
        [ReactProp("overflow")]
        public void SetOverflow(TFrameworkElement view, string overflow)
        {
#if WINDOWS_UWP
            if (overflow == "hidden")
            {
                WinRTXamlToolkit.Controls.Extensions.FrameworkElementExtensions.SetClipToBounds(view, true);
            }
            else
            {
                WinRTXamlToolkit.Controls.Extensions.FrameworkElementExtensions.SetClipToBounds(view, false);
            }
#endif
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
        /// Sets the accessibility label of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="label">The label.</param>
        [ReactProp("accessibilityLabel")]
        public void SetAccessibilityLabel(TFrameworkElement view, string label)
        {
            AutomationProperties.SetName(view, label ?? "");
        }

        /// <summary>
        /// Sets the accessibility live region.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="liveRegion">The live region.</param>
        [ReactProp("accessibilityLiveRegion")]
        public void SetAccessibilityLiveRegion(TFrameworkElement view, string liveRegion)
        {
#if WINDOWS_UWP
            var liveSetting = AutomationLiveSetting.Off;
            switch (liveRegion)
            {
                case "polite":
                    liveSetting = AutomationLiveSetting.Polite;
                    break;
                case "assertive":
                    liveSetting = AutomationLiveSetting.Assertive;
                    break;
            }

            AutomationProperties.SetLiveSetting(view, liveSetting);
#endif
        }

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

        private static void SetProjectionMatrix(TFrameworkElement view, JArray transforms)
        {
#if WINDOWS_UWP
            var projection = EnsureProjection(view);
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

            projection.ProjectionMatrix = translateMatrix * transformMatrix * translateBackMatrix;
#endif
        }

        private static void ResetProjectionMatrix(TFrameworkElement view)
        {
#if WINDOWS_UWP
            var projection = view.Projection;
            var matrixProjection = projection as Matrix3DProjection;
            if (projection != null && matrixProjection == null)
            {
                throw new InvalidOperationException("Unknown projection set on framework element.");
            }

            view.Projection = null;
#endif
        }

#if WINDOWS_UWP
        private static Matrix3DProjection EnsureProjection(FrameworkElement view)
        {
            var projection = view.Projection;
            var matrixProjection = projection as Matrix3DProjection;
            if (projection != null && matrixProjection == null)
            {
                throw new InvalidOperationException("Unknown projection set on framework element.");
            }

            if (matrixProjection == null)
            {
                matrixProjection = new Matrix3DProjection();
                view.Projection = matrixProjection;
            }

            return matrixProjection;
        }
#endif
    }
}
