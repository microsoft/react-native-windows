// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using ReactNative.UIManager;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using static System.FormattableString;

namespace ReactNative.Views.Modal
{
    /// <summary>
    /// A view manager responsible for rendering modals.
    /// </summary>
    public class ReactModalViewManager : ViewParentManager<ReactModalHostView, ReactModalShadowNode>
    {
        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTModalHostView";
            }
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(ReactModalHostView parent, DependencyObject child, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), Invariant($"{nameof(ContentDialog)} currently only supports one child."));
            }
            if (parent.Content != null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ContentDialog)} already has a child element."));
            }
            
            parent.Content = child;
        }

        /// <summary>
        /// This method should return the <see cref="ReactModalShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view. 
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactModalShadowNode CreateShadowNodeInstance()
        {
            return new ReactModalShadowNode();
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public override DependencyObject GetChildAt(ReactModalHostView parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), $"{nameof(ContentDialog)} currently only supports one child.");
            }

            return EnsureChild(parent);
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(ReactModalHostView parent)
        {
            return parent.Content != null ? 1 : 0;
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, ReactModalHostView view)
        {
            base.OnDropViewInstance(reactContext, view);

            view.Hide();
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(ReactModalHostView parent)
        {
            parent.Content = null;
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(ReactModalHostView parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), $"{nameof(ContentDialog)} currently only supports one child.");
            }

            EnsureChild(parent);
            RemoveAllChildren(parent);
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override ReactModalHostView CreateViewInstance(ThemedReactContext reactContext)
        {
            return new ReactModalHostView();
        }

        private static DependencyObject EnsureChild(ReactModalHostView view)
        {
            var child = view.Content;
            if (child == null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ContentDialog)} does not have any children."));
            }

            var dependencyObject = child as DependencyObject;
            if (dependencyObject == null)
            {
                throw new InvalidOperationException(Invariant($"Invalid child element in {nameof(ContentDialog)}."));
            }

            return dependencyObject;
        }
    }
}
