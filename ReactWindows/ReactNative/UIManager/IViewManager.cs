using System.Collections.Generic;
using Newtonsoft.Json.Linq;
using Windows.UI.Xaml;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Base, non-generic interface for view managers.
    /// </summary>
    public interface IViewManager
    {
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        string Name { get; }

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
        /// The native properties.
        /// </summary>
        IReadOnlyDictionary<string, string> NativeProperties { get; }

        /// <summary>
        /// Creates a shadow node for the view manager.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        ReactShadowNode CreateShadowNodeInstance();

        /// <summary>
        /// Creates a view and installs event emitters on it.
        /// </summary>
        /// <param name="reactContext">The context.</param>
        /// <param name="jsResponderHandler">The responder handler.</param>
        /// <returns>The view.</returns>
        FrameworkElement CreateView(ThemedReactContext themedContext, JavaScriptResponderHandler jsResponderHandler);

        /// <summary>
        /// Implement this method to receive events/commands directly from
        /// JavaScript through the <see cref="UIManager"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        void ReceiveCommand(FrameworkElement view, int commandId, JArray args);

        /// <summary>
        /// Update the properties of the given view.
        /// </summary>
        /// <param name="viewToUpdate">The view to update.</param>
        /// <param name="properties">The properties.</param>
        void UpdateProperties(FrameworkElement viewToUpdate, CatalystStylesDiffMap properties);

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        void UpdateExtraData(FrameworkElement viewToUpdate, object extraData);
       
        void OnDropViewInstance(ThemedReactContext themedReactContext, FrameworkElement view);
    }
}
