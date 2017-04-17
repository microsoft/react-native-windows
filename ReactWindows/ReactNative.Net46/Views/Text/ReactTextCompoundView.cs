using ReactNative.UIManager;
using System.Windows;
using System.Windows.Controls;

namespace ReactNative.Views.Text
{
    class ReactTextCompoundView : IReactCompoundView
    {
        public int GetReactTagAtPoint(UIElement reactView, Point point)
        {
            var richTextBlock = reactView.As<TextBlock>();
            var textPointer = richTextBlock.GetPositionFromPoint(point, true);
            return textPointer.Parent.GetTag();
        }
    }
}
