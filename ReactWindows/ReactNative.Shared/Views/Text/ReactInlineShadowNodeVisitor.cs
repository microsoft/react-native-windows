using ReactNative.UIManager;
using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Shapes;
#else
using System.Windows.Documents;
using System.Windows.Shapes;
#endif

namespace ReactNative.Views.Text
{
    class ReactInlineShadowNodeVisitor : ReactShadowNodeVisitor<Inline>
    {
        private static readonly ReactInlineShadowNodeVisitor s_instance = new ReactInlineShadowNodeVisitor();

        public static Inline Apply(ReactShadowNode node)
        {
            return s_instance.Visit(node);
        }

        protected sealed override Inline Make(ReactShadowNode node, IList<Inline> children)
        {
            var textNode = node as ReactInlineShadowNode;
            if (textNode != null)
            {
                return textNode.MakeInline(children);
            }
            else
            {
                var rectangle = new Rectangle();
                rectangle.Width = node.StyleWidth;
                rectangle.Height = node.StyleHeight;
                return new InlineUIContainer
                {
                    Child = rectangle,                    
                };
            }
        }
    }
}
