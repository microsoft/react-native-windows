// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Views.Scroll;

namespace ReactNative.Tests.Views.Scroll
{
    public interface IScrollViewScroller
    {
        void ScrollToHorizontalOffset(double offset);
        void ScrollToVerticalOffset(double offset);
    };

    public class TestScrollView : IScrollView
    {
        private double _horizontalOffset = 0;
        private double _verticalOffset = 0;
        private IScrollViewScroller _scroller;

        public TestScrollView(IScrollViewScroller scroller)
        {
            _scroller = scroller;
        }

        public double HorizontalOffset
        {
            get { return _horizontalOffset; }
            set { _horizontalOffset = value; }
        }

        public double VerticalOffset
        {
            get { return _verticalOffset; }
            set { _verticalOffset = value; }
        }

        public void ScrollToHorizontalOffset(double offset)
        {
            _horizontalOffset = offset;
            _scroller.ScrollToHorizontalOffset(offset);
        }

        public void ScrollToVerticalOffset(double offset)
        {
            _verticalOffset = offset;
            _scroller.ScrollToVerticalOffset(offset);
        }
    }
}
