// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Common base class for most of the <see cref="IViewManager"/> instances. 
    /// It provides support for most common props through extending 
    /// <see cref="BaseViewManager{TFrameworkElement, TLayoutShadowNode}"/>.
    /// </summary>
    /// <typeparam name="TFrameworkElement">Type of framework element.</typeparam>
    public abstract class SimpleViewManager<TFrameworkElement> : BaseViewManager<TFrameworkElement, LayoutShadowNode>
        where TFrameworkElement : FrameworkElement
    {
        /// <summary>
        /// Creates a <see cref="LayoutShadowNode"/> instance.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public sealed override LayoutShadowNode CreateShadowNodeInstance()
        {
            return new LayoutShadowNode();
        }

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(TFrameworkElement root, object extraData)
        {
        }
    }
}
