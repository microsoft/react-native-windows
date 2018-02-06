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
    /// An interface for extending view managers with additional props.
    /// </summary>
    public interface IViewManagerExtension
    {
        /// <summary>
        /// The type of view manager type being extended.
        /// </summary>
        Type ViewManagerType { get; }

        /// <summary>
        /// The commands map for the view manager.
        /// </summary>
        IReadOnlyDictionary<string, object> CommandsMap { get; }

        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants { get; }

        /// <summary>
        /// The exported custom direct event types.
        /// </summary>
        IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants { get; }

        /// <summary>
        /// The exported view constants.
        /// </summary>
        IReadOnlyDictionary<string, object> ExportedViewConstants { get; }

        /// <summary>
        /// The native props for the view manager.
        /// </summary>
        IReadOnlyDictionary<string, string> NativeProperties { get; }

        /// <summary>
        /// Called when the view is created.
        /// </summary>
        /// <param name="reactContext">The context.</param>
        /// <param name="view">The view.</param>
        void OnCreateView(ThemedReactContext reactContext, DependencyObject view);

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
        void OnDropViewInstance(ThemedReactContext reactContext, DependencyObject view);

        /// <summary>
        /// Implement this method to receive events/commands directly from
        /// JavaScript through the <see cref="UIManager"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        void ReceiveCommand(DependencyObject view, int commandId, JArray args);

        /// <summary>
        /// Update the properties of the given view.
        /// </summary>
        /// <param name="viewToUpdate">The view to update.</param>
        /// <param name="props">The properties.</param>
        void UpdateProperties(DependencyObject viewToUpdate, ReactStylesDiffMap props);
    }
}
