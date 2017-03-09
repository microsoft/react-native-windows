using System;
using ReactNative.UIManager;
using System.Collections.Generic;
using Facebook.Yoga;
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

            if (node.StyleWidth.Unit != YogaUnit.Point || node.StyleHeight.Unit != YogaUnit.Point)
            {
                throw new InvalidOperationException("Inline views embedded in text must have absolute dimensions.");
            }

            var rectangle = new Rectangle
            {
                Width = node.StyleWidth.Value,
                Height = node.StyleHeight.Value,
            };
            
            return new InlineUIContainer
            {
                Child = rectangle,                    
            };
        }
    }
}
