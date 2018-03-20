// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NMock;
using NUnit.Framework;
using ReactNative.Views.Scroll;
using System;
using System.Threading.Tasks;

namespace ReactNative.Tests.Views.Scroll
{
    [TestFixture]
    class ReactScrollViewManagerTests
    {
        private MockFactory _mockFactory;
        private TestScrollView _testScrollView;
        private Mock<IScrollViewScroller> _mockScrollViewScroller;
        private ReactScrollViewManager _scrollViewManager;

        [SetUp]
        public void SetUp()
        {
            _mockFactory = new MockFactory();
            _mockScrollViewScroller = _mockFactory.CreateMock<IScrollViewScroller>();
            _testScrollView = new TestScrollView(_mockScrollViewScroller.MockObject);
            _scrollViewManager = new ReactScrollViewManager();
        }

        [Test]

        public async Task ReactScrollViewManager_ScrollAnimated_VerticalOnly()
        {
            // Arrange
            _testScrollView.HorizontalOffset = 0;
            _testScrollView.VerticalOffset = 0;

            _mockScrollViewScroller.Expects
                .No
                .Method(_ => _.ScrollToHorizontalOffset(0));
            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 0 && offset <= 100));

            // Act
            await _scrollViewManager.ScrollToAnimated(new WeakReference<IScrollView>(_testScrollView), 0, 100);

            // Assert
            Assert.That(_testScrollView.HorizontalOffset, NUnit.Framework.Is.EqualTo(0));
            Assert.That(_testScrollView.VerticalOffset, NUnit.Framework.Is.EqualTo(100));
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
        }

        [Test]
        public async Task ReactScrollViewManager_ScrollAnimated_HorizontalOnly()
        {
            // Arrange
            _testScrollView.HorizontalOffset = 0;
            _testScrollView.VerticalOffset = 0;

            _mockScrollViewScroller.Expects
                .No
                .Method(_ => _.ScrollToVerticalOffset(0));
            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToHorizontalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 0 && offset <= 100));

            // Act
            await _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 100, 0);

            // Assert
            Assert.That(_testScrollView.VerticalOffset, NUnit.Framework.Is.EqualTo(0));
            Assert.That(_testScrollView.HorizontalOffset, NUnit.Framework.Is.EqualTo(100));
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
        }

        [Test]
        public async Task ReactScrollViewManager_ScrollAnimated_HorizontalAndVertical()
        {
            // Arrange
            _testScrollView.HorizontalOffset = 0;
            _testScrollView.VerticalOffset = 0;

            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToHorizontalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 0 && offset <= 50));
            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 0 && offset <= 100));

            // Act
            await _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 50, 100);

            // Assert
            Assert.That(_testScrollView.HorizontalOffset, NUnit.Framework.Is.EqualTo(50));
            Assert.That(_testScrollView.VerticalOffset, NUnit.Framework.Is.EqualTo(100));
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
        }

        [Test]
        [DefaultFloatingPointTolerance(0.01)]
        public async Task ReactScrollViewManager_ScrollAnimated_Reentrant()
        {
            // Arrange
            _testScrollView.HorizontalOffset = 0;
            _testScrollView.VerticalOffset = 0;

            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 0 && offset <= 100));

            // Act
            var task1ExpectToBeCancelled = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 100);
            await Task.Delay(10);
            var task2ReentrantExpectToFinish = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 30);

            // Assert
            Assert.That(async () => await task1ExpectToBeCancelled, Throws.InstanceOf<OperationCanceledException>());
            await task2ReentrantExpectToFinish;

            Assert.That(_testScrollView.HorizontalOffset, NUnit.Framework.Is.EqualTo(0));
            Assert.That(_testScrollView.VerticalOffset, NUnit.Framework.Is.EqualTo(30));
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
        }

        [Test]
        [DefaultFloatingPointTolerance(0.01)]
        public async Task ReactScrollViewManager_ScrollAnimated_ReentrantTwice()
        {
            // Arrange
            _testScrollView.HorizontalOffset = 0;
            _testScrollView.VerticalOffset = 0;

            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 0 && offset <= 300));

            // Act
            var task1ExpectToBeCancelled = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 100);
            await Task.Delay(10);
            var task2ReentrantExpectToBeCancelled = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 30);
            var task3ReentrantExpectToFinish = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 300);

            // Assert
            Assert.That(async () => await task1ExpectToBeCancelled, Throws.InstanceOf<OperationCanceledException>());
            Assert.That(async () => await task2ReentrantExpectToBeCancelled, Throws.InstanceOf<OperationCanceledException>());
            await task3ReentrantExpectToFinish;

            Assert.That(_testScrollView.HorizontalOffset, NUnit.Framework.Is.EqualTo(0));
            Assert.That(_testScrollView.VerticalOffset, NUnit.Framework.Is.EqualTo(300));
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
        }

        [Test]
        [DefaultFloatingPointTolerance(0.01)]
        public async Task ReactScrollViewManager_ScrollAnimated_ReentrantForthAndBack()
        {
            // Arrange
            _testScrollView.HorizontalOffset = 0;
            _testScrollView.VerticalOffset = 0;

            // Expect scroller will scroll to the position exceeding |task2ReentrantWithSmallOffset|
            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 100 && offset <= 1000));

            // Expect scroller will then scroll back to the position within |task2ReentrantWithSmallOffset|
            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset <= 10));

            // Act
            var task1WithLargeTargetOffset = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 1000);
            // Wait 100ms to simulate the reentrant timing in the middle of ScrollAnimated
            await Task.Delay(100);
            var task2ReentrantWithSmallOffset = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 10);

            // Assert
            Assert.That(async () => await task1WithLargeTargetOffset, Throws.InstanceOf<OperationCanceledException>());
            await task2ReentrantWithSmallOffset;

            Assert.That(_testScrollView.HorizontalOffset, NUnit.Framework.Is.EqualTo(0));
            Assert.That(_testScrollView.VerticalOffset, NUnit.Framework.Is.EqualTo(10));
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
        }
    }
}
