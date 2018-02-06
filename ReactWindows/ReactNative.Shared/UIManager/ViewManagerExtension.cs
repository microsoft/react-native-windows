using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Base class for view manager extensions.
    /// </summary>
    /// <typeparam name="TDependencyObject">Type of dependency object.</typeparam>
    /// <typeparam name="TViewManager">Type of view manager.</typeparam>
    public class ViewManagerExtension<TDependencyObject, TViewManager> : IViewManagerExtension
        where TDependencyObject : DependencyObject
        where TViewManager : IViewManager
    {
        /// <summary>
        /// The type of view manager type being extended.
        /// </summary>
        public Type ViewManagerType => typeof(TViewManager);

        /// <summary>
        /// The commands map for the view manager extension.
        /// </summary>
        public virtual IReadOnlyDictionary<string, object> CommandsMap { get; }

        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        public virtual IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants { get; }

        /// <summary>
        /// The exported custom direct event types.
        /// </summary>
        public virtual IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants { get; }

        /// <summary>
        /// The exported view constants.
        /// </summary>
        public virtual IReadOnlyDictionary<string, object> ExportedViewConstants { get; }

        /// <summary>
        /// The native props for the view manager extension.
        /// </summary>
        public IReadOnlyDictionary<string, string> NativeProperties
        {
            get
            {
                return ViewManagersPropertyCache.GetNativePropertiesForView(GetType(), typeof(ReactShadowNode));
            }
        }

        /// <summary>
        /// Called when the view is created.
        /// </summary>
        /// <param name="reactContext">The context.</param>
        /// <param name="view">The view.</param>
        public virtual void OnCreateView(ThemedReactContext reactContext, TDependencyObject view)
        {
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="IViewManagerExtension"/>
        /// subclass.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        /// <remarks>
        /// Derived classes do not need to call this base method.
        /// </remarks>
        public virtual void OnDropViewInstance(ThemedReactContext reactContext, TDependencyObject view)
        {
        }

        /// <summary>
        /// Implement this method to receive events/commands directly from
        /// JavaScript through the <see cref="UIManager"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifier for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public virtual void ReceiveCommand(TDependencyObject view, int commandId, JArray args)
        {
        }

        /// <summary>
        /// Update the properties of the given view.
        /// </summary>
        /// <param name="viewToUpdate">The view to update.</param>
        /// <param name="props">The properties.</param>
        public void UpdateProperties(DependencyObject viewToUpdate, ReactStylesDiffMap props)
        {
            var propertySetters =
                ViewManagersPropertyCache.GetNativePropertySettersForViewManagerType(GetType());

            var keys = props.Keys;
            foreach (var key in keys)
            {
                var setter = default(IPropertySetter);
                if (propertySetters.TryGetValue(key, out setter))
                {
                    setter.UpdateViewManagerExtensionProperty(this, viewToUpdate, props);
                }
            }

            OnAfterUpdateTransaction((TDependencyObject)viewToUpdate);
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
        /// </remarks>
        protected virtual void AddEventEmitters(ThemedReactContext reactContext, TDependencyObject view)
        {
        }

        /// <summary>
        /// Callback that will be triggered after all properties are updated in
        /// the current update transation (all <see cref="Annotations.ReactPropAttribute"/> handlers
        /// for properties updated in the current transaction have been called).
        /// </summary>
        /// <param name="view">The view.</param>
        protected virtual void OnAfterUpdateTransaction(TDependencyObject view)
        {
        }

        #region IViewManagerExtension

        void IViewManagerExtension.OnCreateView(ThemedReactContext reactContext, DependencyObject view)
        {
            OnCreateView(reactContext, (TDependencyObject)view);
        }

        void IViewManagerExtension.OnDropViewInstance(ThemedReactContext reactContext, DependencyObject view)
        {
            OnDropViewInstance(reactContext, (TDependencyObject)view);
        }

        void IViewManagerExtension.ReceiveCommand(DependencyObject view, int commandId, JArray args)
        {
            ReceiveCommand((TDependencyObject)view, commandId, args);
        }

        #endregion
    }
}
