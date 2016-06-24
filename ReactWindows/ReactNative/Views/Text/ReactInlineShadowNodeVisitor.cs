using Facebook.CSSLayout;
using ReactNative.UIManager;
using System.Collections.Generic;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Shapes;

namespace ReactNative.Views.Text
{
    class ReactInlineShadowNodeVisitor : CSSNodeVisitor<Inline>
    {
        private static readonly ReactInlineShadowNodeVisitor s_instance = new ReactInlineShadowNodeVisitor();

        public static Inline Apply(CSSNode node)
        {
            return s_instance.Visit(node);
        }

        protected sealed override Inline Make(CSSNode node, IList<Inline> children)
        {
            var textNode = node as ReactInlineShadowNode;
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
