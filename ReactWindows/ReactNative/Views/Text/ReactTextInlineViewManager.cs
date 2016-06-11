using ReactNative.UIManager;
using Windows.UI.Xaml.Documents;

namespace ReactNative.Views.Text
{
    public abstract class ReactTextInlineViewManager<TInline, TReactTextInlineShadowNode> : DependencyObjectViewManager<TInline, TReactTextInlineShadowNode>
        where TInline : Inline
        where TReactTextInlineShadowNode : ReactTextInlineShadowNode
    {
        public sealed override void UpdateExtraData(TInline root, object extraData)
        {
            var inlineNode = extraData as ReactTextInlineShadowNode;
            if (inlineNode != null)
            {
                inlineNode.UpdateInline(root);
            }
        }
    }
}
