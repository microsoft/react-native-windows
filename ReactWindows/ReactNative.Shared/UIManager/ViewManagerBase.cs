// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Json;
using System;
using System.Collections.Generic;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Class responsible for knowing how to create and update views of a given
    /// type. It is also responsible for creating and updating
    /// <see cref="ReactShadowNode"/> subclasses used for calculating position
    /// and size for the corresponding native view.
    /// </summary>
    public abstract class ViewManagerBase<TView, TReactShadowNode> : IViewManager
        where TReactShadowNode : ReactShadowNode
    {
        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public abstract string Name { get; }

        /// <summary>
        /// The <see cref="Type"/> instance that represents the type of shadow
        /// node that this manager will return from
        /// <see cref="CreateShadowNodeInstance"/>.
        /// 
        /// This method will be used in the bridge initialization phase to
        /// collect props exposed using the <see cref="Annotations.ReactPropAttribute"/>
        /// annotation from the <see cref="ReactShadowNode"/> subclass.
        /// </summary>
        public virtual Type ShadowNodeType => typeof(TReactShadowNode);

        /// <summary>
        /// The commands map for the view manager.
        /// </summary>
        [Obsolete("Use 'ViewCommandsMap' instead.")]
        public virtual IReadOnlyDictionary<string, object> CommandsMap { get; }

        /// <summary>
        /// The commands map for the view manager.
        /// </summary>
        public virtual JObject ViewCommandsMap { get; }

        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        [Obsolete("Use 'CustomBubblingEventTypeConstants' instead.")]
        public virtual IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants { get; }

        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        public virtual JObject CustomBubblingEventTypeConstants { get; }

        /// <summary>
        /// The exported custom direct event types.
        /// </summary>
        [Obsolete("Use 'CustomDirectEventTypeConstants' instead.")]
        public virtual IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants { get; }

        /// <summary>
        /// The exported custom direct event types.
        /// </summary>
        public virtual JObject CustomDirectEventTypeConstants { get; }

        /// <summary>
        /// The exported view constants.
        /// </summary>
        [Obsolete("Use 'ViewConstants' instead.")]
        public virtual IReadOnlyDictionary<string, object> ExportedViewConstants { get; }

        /// <summary>
        /// The exported view constants.
        /// </summary>
        public virtual JObject ViewConstants { get; }

        /// <summary>
        /// Creates a view and installs event emitters on it.
        /// </summary>
        /// <param name="reactContext">The context.</param>
        /// <returns>The view.</returns>
        public virtual TView CreateView(ThemedReactContext reactContext)
        {
            var view = CreateViewInstance(reactContext);
            OnViewInstanceCreated(reactContext, view);
            AddEventEmitters(reactContext, view);
            // TODO: enable touch intercepting view parents
            return view;
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="IViewManager"/> subclass.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public virtual void OnDropViewInstance(ThemedReactContext reactContext, TView view)
        {
        }

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
        public abstract TReactShadowNode CreateShadowNodeInstance();

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public abstract void UpdateExtraData(TView root, object extraData);

        /// <summary>
        /// Implement this method to receive events/commands directly from
        /// JavaScript through the <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public virtual void ReceiveCommand(TView view, int commandId, JArray args)
        {
        }

        /// <summary>
        /// Gets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>The view dimensions.</returns>
        public abstract Dimensions GetDimensions(TView view);

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The output buffer.</param>
        public abstract void SetDimensions(TView view, Dimensions dimensions);

        /// <summary>
        /// Creates a new view instance of type <typeparamref name="TView"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected abstract TView CreateViewInstance(ThemedReactContext reactContext);

        /// <summary>
        /// Subclasses can override this method to install custom event 
        /// emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        /// <remarks>
        /// Consider overriding this method if your view needs to emit events
        /// besides basic touch events to JavaScript (e.g., scroll events).
        /// </remarks>
        protected virtual void AddEventEmitters(ThemedReactContext reactContext, TView view)
        {
        }

        /// <summary>
        /// Callback that will be triggered after all props are updated in
        /// the current update transation (all <see cref="Annotations.ReactPropAttribute"/> handlers
        /// for props updated in the current transaction have been called).
        /// </summary>
        /// <param name="view">The view.</param>
        protected virtual void OnAfterUpdateTransaction(TView view)
        {
        }

        internal virtual void OnViewInstanceCreated(ThemedReactContext reactContext, TView view)
        {
        }

        #region IViewManager

        JObject IViewManager.NativeProps
        {
            get
            {
                return ViewManagersPropCache.GetNativePropsForView<TView>(GetType(), ShadowNodeType);
            }
        }

#pragma warning disable CS0618 // Type or member is obsolete
        JObject IViewManager.CommandsMap
        {
            get
            {
                return OneOf(ViewCommandsMap, CommandsMap);
            }
        }

        JObject IViewManager.ExportedCustomBubblingEventTypeConstants
        {
            get
            {
                return OneOf(CustomBubblingEventTypeConstants, ExportedCustomBubblingEventTypeConstants);
            }
        }

        JObject IViewManager.ExportedCustomDirectEventTypeConstants
        {
            get
            {
                return OneOf(CustomDirectEventTypeConstants, ExportedCustomDirectEventTypeConstants);
            }
        }

        JObject IViewManager.ExportedViewConstants
        {
            get
            {
                return OneOf(ViewConstants, ExportedViewConstants);
            }
        }
#pragma warning restore CS0618 // Type or member is obsolete

        void IViewManager.UpdateProps(object viewToUpdate, JObject props)
        {
            var propSetters =
                ViewManagersPropCache.GetNativePropSettersForViewManagerType<TView>(GetType());

            var keys = props.Keys();
            foreach (var key in keys)
            {
                var setter = default(IPropSetter);
                if (propSetters.TryGetValue(key, out setter))
                {
                    setter.UpdateViewManagerProp(this, viewToUpdate, props);
                }
            }

            OnAfterUpdateTransaction((TView)viewToUpdate);
        }

        object IViewManager.CreateView(ThemedReactContext reactContext)
        {
            return CreateView(reactContext);
        }

        void IViewManager.OnDropViewInstance(ThemedReactContext reactContext, object view)
        {
            OnDropViewInstance(reactContext, (TView)view);
        }

        ReactShadowNode IViewManager.CreateShadowNodeInstance()
        {
            return CreateShadowNodeInstance();
        }

        void IViewManager.UpdateExtraData(object root, object extraData)
        {
            UpdateExtraData((TView)root, extraData);
        }

        void IViewManager.ReceiveCommand(object view, int commandId, JArray args)
        {
            ReceiveCommand((TView)view, commandId, args);
        }

        Dimensions IViewManager.GetDimensions(object view)
        {
            return GetDimensions((TView)view);
        }

        void IViewManager.SetDimensions(object view, Dimensions dimensions)
        {
            SetDimensions((TView)view, dimensions);
        }

        #endregion

        #region Constants Helpers

        private static JObject OneOf(JObject json, IReadOnlyDictionary<string, object> map)
        {
            if (map != null && json != null)
            {
                throw new NotSupportedException("Do not override both JObject and dictionary constants properties.");
            }

            return json ?? (map != null ? JObject.FromObject(map) : null);
        }

        #endregion
    }
}
