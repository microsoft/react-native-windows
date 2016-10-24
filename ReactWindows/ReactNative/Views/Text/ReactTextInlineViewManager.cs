using ReactNative.UIManager;
using Windows.UI.Xaml.Documents;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// Base view manager class for text block inlines.
    /// </summary>
    /// <typeparam name="TInline">Type of inline.</typeparam>
    /// <typeparam name="TReactTextInlineShadowNode">Type of shadow node.</typeparam>
    public abstract class ReactTextInlineViewManager<TInline, TReactTextInlineShadowNode> : DependencyObjectViewManager<TInline, TReactTextInlineShadowNode>
        where TInline : Inline
        where TReactTextInlineShadowNode : ReactInlineShadowNode
    {
        /// <summary>
        /// Receive extra updates from the shadow node.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public sealed override void UpdateExtraData(TInline root, object extraData)
        {
            var inlineNode = extraData as ReactInlineShadowNode;
            if (inlineNode != null)
            {
                inlineNode.UpdateInline(root);
            }
        }
    }
}
