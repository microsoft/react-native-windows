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

        [TearDown]
        public void TearDown()
        {
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
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
            Assert.AreEqual(0, _testScrollView.HorizontalOffset, 0.01);
            Assert.AreEqual(100, _testScrollView.VerticalOffset, 0.01);
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
            Assert.AreEqual(0, _testScrollView.VerticalOffset, 0.01);
            Assert.AreEqual(100, _testScrollView.HorizontalOffset, 0.01);
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
            Assert.AreEqual(50, _testScrollView.HorizontalOffset, 0.01);
            Assert.AreEqual(100, _testScrollView.VerticalOffset, 0.01);
        }

        [Test]
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
            var task1 = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 100);
            await Task.Delay(10);
            var task2 = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 30);

            // Assert
            Assert.That(async () => await task1, Throws.InstanceOf<TaskCanceledException>());
            await task2;

            Assert.AreEqual(0, _testScrollView.HorizontalOffset, 0.01);
            Assert.AreEqual(30, _testScrollView.VerticalOffset, 0.01);
        }

        [Test]
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
            var task1 = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 100);
            await Task.Delay(10);
            var task2 = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 30);
            var task3 = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 300);

            // Assert
            Assert.That(async () => await task1, Throws.InstanceOf<TaskCanceledException>());
            Assert.That(async () => await task2, Throws.InstanceOf<TaskCanceledException>());
            await task3;

            Assert.AreEqual(0, _testScrollView.HorizontalOffset, 0.01);
            Assert.AreEqual(300, _testScrollView.VerticalOffset, 0.01);
        }

        [Test]
        public async Task ReactScrollViewManager_ScrollAnimated_ReentrantForthAndBack()
        {
            // Arrange
            _testScrollView.HorizontalOffset = 0;
            _testScrollView.VerticalOffset = 0;

            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset > 100 && offset <= 1000));

            _mockScrollViewScroller.Expects
                .AtLeastOne
                .Method(_ => _.ScrollToVerticalOffset(0))
                .With(NMock.Is.Match<double>(offset => offset <= 10));

            // Act
            var task1 = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 1000);
            await Task.Delay(100);
            var task2 = _scrollViewManager.ScrollToAnimated(
                new WeakReference<IScrollView>(_testScrollView), 0, 10);

            // Assert
            Assert.That(async () => await task1, Throws.InstanceOf<TaskCanceledException>());
            await task2;

            Assert.AreEqual(0, _testScrollView.HorizontalOffset, 0.01);
            Assert.AreEqual(10, _testScrollView.VerticalOffset, 0.01);
        }
    }
}
