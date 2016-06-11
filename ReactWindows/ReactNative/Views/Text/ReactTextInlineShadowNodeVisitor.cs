using Facebook.CSSLayout;
using ReactNative.UIManager;
using System.Collections.Generic;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Shapes;

namespace ReactNative.Views.Text
{
    class ReactTextInlineShadowNodeVisitor : CSSNodeVisitor<Inline>
    {
        private static readonly ReactTextInlineShadowNodeVisitor s_instance = new ReactTextInlineShadowNodeVisitor();

        public static Inline Apply(CSSNode node)
        {
            return s_instance.Visit(node);
        }

        protected sealed override Inline Make(CSSNode node, IList<Inline> children)
        {
            var textNode = node as ReactTextInlineShadowNode;
            if (textNode != null)
            {
                return textNode.MakeInline(children);
            }
            else
            {
                var rectangle = new Rectangle();
                rectangle.Width = node.Width;
                rectangle.Height = node.Height;
                return new InlineUIContainer
                {
                    Child = rectangle,                    
                };
            }
        }
    }
}
