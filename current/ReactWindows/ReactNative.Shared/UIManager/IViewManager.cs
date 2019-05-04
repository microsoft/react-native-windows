// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Interface responsible for knowing how to create and update views of a given
    /// type. It is also responsible for creating and updating
    /// <see cref="ReactShadowNode"/> subclasses used for calculating position
    /// and size for the corresponding native view.
    /// </summary>
    public interface IViewManager
    {
        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        string Name { get; }

        /// <summary>
        /// The <see cref="Type"/> instance that represents the type of shadow
        /// node that this manager will return from
        /// <see cref="CreateShadowNodeInstance"/>.
        /// 
        /// This method will be used in the bridge initialization phase to
        /// collect props exposed using the <see cref="Annotations.ReactPropAttribute"/>
        /// annotation from the <see cref="ReactShadowNode"/> subclass.
        /// </summary>
        Type ShadowNodeType { get; }

        /// <summary>
        /// The commands map for the view manager.
        /// </summary>
        JObject CommandsMap { get; }

        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        JObject ExportedCustomBubblingEventTypeConstants { get; }

        /// <summary>
        /// The exported custom direct event types.
        /// </summary>
        JObject ExportedCustomDirectEventTypeConstants { get; }

        /// <summary>
        /// The exported view constants.
        /// </summary>
        JObject ExportedViewConstants { get; }

        /// <summary>
        /// The native props for the view manager.
        /// </summary>
        JObject NativeProps { get; }

        /// <summary>
        /// Update the props of the given view.
        /// </summary>
        /// <param name="viewToUpdate">The view to update.</param>
        /// <param name="props">The props.</param>
        void UpdateProps(object viewToUpdate, JObject props);

        /// <summary>
        /// Creates a view and installs event emitters on it.
        /// </summary>
        /// <param name="reactContext">The context.</param>
        /// <returns>The view.</returns>
        object CreateView(ThemedReactContext reactContext);

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="IViewManager"/>
        /// subclass.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        /// <remarks>
        /// Derived classes do not need to call this base method.
        /// </remarks>
        void OnDropViewInstance(ThemedReactContext reactContext, object view);

        /// <summary>
        /// This method should return the subclass of <see cref="ReactShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view. 
        /// </summary>
        /// <remarks>
        /// In most cases, this will just return an instance of
        /// <see cref="ReactShadowNode"/>.
        /// </remarks>
        /// <returns>The shadow node instance.</returns>
        ReactShadowNode CreateShadowNodeInstance();

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        void UpdateExtraData(object root, object extraData);

        /// <summary>
        /// Implement this method to receive events/commands directly from
        /// JavaScript through the <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        void ReceiveCommand(object view, int commandId, JArray args);

        /// <summary>
        /// Gets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The view dimensions.</returns>
        Dimensions GetDimensions(object view);

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The dimensions.</param>
        void SetDimensions(object view, Dimensions dimensions);
    }
}
