using ReactNative.UIManager;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Views.Text
{
    class ReactTextCompoundView : IReactCompoundView
    {
        public int GetReactTagAtPoint(UIElement reactView, Point point)
        {
            var richTextBlock = reactView.As<RichTextBlock>();
            var textPointer = richTextBlock.GetPositionFromPoint(point);
            var textPointerParent = textPointer.Parent;

            // We may encounter a case where the parent item is the
            // `Paragraph` inline attached to the `RichTextBlock`.
            // In this case, simply return the tag of the `RichTextBlock`.
            return textPointerParent.HasTag()
                ? textPointerParent.GetTag()
                : richTextBlock.GetTag();
        }
    }
}
