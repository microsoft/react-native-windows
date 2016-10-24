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
            return textPointer.Parent.GetTag();
        }
    }
}
