// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.UIManager;
using System.Linq;
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
            var parentView = RootViewHelper.GetReactViewHierarchy(textPointer.Parent).First();
            return parentView.GetTag();
        }
    }
}
