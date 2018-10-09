// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.ViewManagement;
using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.Modal
{
    /// <summary>
    /// The shadow node implementation for Picker views.
    /// </summary>
    public class ReactModalShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactModalShadowNode"/>.
        /// </summary>
        public ReactModalShadowNode()
            : base(false, true)
        {
            ApplicationView.GetForCurrentView().VisibleBoundsChanged += ApplicationViewOnVisibleBoundsChanged;
        }

        /// <summary>
        /// Disposes the shadow node.
        /// </summary>
        public override void Dispose()
        {
            base.Dispose();

            ApplicationView.GetForCurrentView().VisibleBoundsChanged -= ApplicationViewOnVisibleBoundsChanged;
        }

        /// <summary>
        /// Insert a child at the given index.
        /// </summary>
        /// <param name="child">The child.</param>
        /// <param name="index">The index.</param>
        public override void AddChildAt(ReactShadowNode child, int index)
        {
            base.AddChildAt(child, index);

            UpdateSize(child);
        }

        private void ApplicationViewOnVisibleBoundsChanged(ApplicationView sender, object args)
        {
            for (var i = 0; i < ChildCount; i++)
                UpdateSize(GetChildAt(i));
        }

        private static void UpdateSize(ReactShadowNode child)
        {
            var screenBounds = ApplicationView.GetForCurrentView().VisibleBounds;
            child.StyleHeight = YogaValue.Point((float)screenBounds.Height);
            child.StyleWidth = YogaValue.Point((float)screenBounds.Width);
        }
    }
}
