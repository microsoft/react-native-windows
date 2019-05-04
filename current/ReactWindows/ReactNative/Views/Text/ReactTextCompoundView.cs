// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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

            // We may encounter a case where the parent object is the
            // `Paragraph` inline attached to the `RichTextBlock`. This is a
            // native implementation detail of the `Text` component, and as 
            // such the `Paragraph` would not have a React tag associated.
            // In this case, simply return the tag of the `RichTextBlock`.
            return textPointerParent != null && textPointerParent.HasTag()
                ? textPointerParent.GetTag()
                : richTextBlock.GetTag();
        }
    }
}
