// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Class responsible for knowing how to create and update views of a given
    /// type. It is also responsible for creating and updating
    /// <see cref="ReactShadowNode"/> subclasses used for calculating position
    /// and size for the corresponding native view.
    /// </summary>
    /// <typeparam name="TFrameworkElement">The view type.</typeparam>
    /// <typeparam name="TReactShadowNode">The shadow node type.</typeparam>
    public abstract class ViewManager<TFrameworkElement, TReactShadowNode> : ViewManagerBase<TFrameworkElement, TReactShadowNode>
        where TFrameworkElement : FrameworkElement
        where TReactShadowNode : ReactShadowNode
    {
        /// <summary>
        /// Gets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The view dimensions.</returns>
        public sealed override Dimensions GetDimensions(TFrameworkElement view)
        {
            return new Dimensions
            {
                X = Canvas.GetLeft(view),
                Y = Canvas.GetTop(view),
                Width = view.Width,
                Height = view.Height,
            };
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The output buffer.</param>
        public override void SetDimensions(TFrameworkElement view, Dimensions dimensions)
        {
            Canvas.SetLeft(view, dimensions.X);
            Canvas.SetTop(view, dimensions.Y);
            view.Width = dimensions.Width;
            view.Height = dimensions.Height;
        }
    }
}
