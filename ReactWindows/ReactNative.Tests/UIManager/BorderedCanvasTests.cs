using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.UIManager;
using System;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Tests.UIManager
{
    using UITestMethodAttribute = Microsoft.VisualStudio.TestPlatform.UnitTestFramework.AppContainer.UITestMethodAttribute;

    [TestClass]
    public class BorderedCanvasTests
    {
        [UITestMethod]
        public void BorderedCanvas_ArgumentExceptions()
        {
            var canvas = new BorderedCanvas();

            AssertEx.Throws<ArgumentNullException>(
                () => canvas.Children.CopyTo(null, 0),
                ex => Assert.AreEqual("array", ex.ParamName));

            AssertEx.Throws<ArgumentOutOfRangeException>(
                () => canvas.Children.RemoveAt(-1),
                ex => Assert.AreEqual("index", ex.ParamName));

            AssertEx.Throws<ArgumentOutOfRangeException>(
                () => canvas.Children.Insert(-1, null),
                ex => Assert.AreEqual("index", ex.ParamName));

            AssertEx.Throws<ArgumentOutOfRangeException>(
                () => canvas.Children.CopyTo(new UIElement[0], -1),
                ex => Assert.AreEqual("arrayIndex", ex.ParamName));

            canvas.Children.Add(new Canvas());

            AssertEx.Throws<ArgumentException>(
                () => canvas.Children.CopyTo(new UIElement[0], 0),
                ex => Assert.AreEqual("array", ex.ParamName));
        }

        [UITestMethod]
        public void BorderedCanvas_CreateBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            Assert.AreEqual(1, ((Canvas)canvas).Children.Count);

            canvas = new BorderedCanvas();
            canvas.BorderThickness = new Thickness(10);
            Assert.AreEqual(1, ((Canvas)canvas).Children.Count);
        }

        [UITestMethod]
        public void BorderedCanvas_TransferBackgroundAndBorderBrush()
        {
            var canvas = new BorderedCanvas();
            var backgroundBrush = new SolidColorBrush(Colors.Blue);
            var borderBrush = new SolidColorBrush(Colors.Red);
            canvas.Background = backgroundBrush;
            canvas.BorderBrush = borderBrush;
            Assert.AreEqual(0, ((Canvas)canvas).Children.Count);

            canvas.BorderThickness = new Thickness(10);
            Assert.AreEqual(1, ((Canvas)canvas).Children.Count);
            var border = ((Canvas)canvas).Children[0] as Border;
            Assert.IsNotNull(border);
            Assert.AreEqual(backgroundBrush, border.Background);
            Assert.AreEqual(borderBrush, border.BorderBrush);

            Assert.AreEqual(null, ((Canvas)canvas).Background);
        }

        [UITestMethod]
        public void BorderedCanvas_AddAfterBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            Assert.AreEqual(0, canvas.Children.Count);
            var child = new Canvas();
            canvas.Children.Add(child);
            Assert.AreEqual(2, ((Canvas)canvas).Children.Count);
            Assert.AreSame(child, canvas.Children[0]);
            Assert.AreSame(child, ((Canvas)canvas).Children[1]);
        }

        [UITestMethod]
        public void BorderedCanvas_AddBeforeBorder()
        {
            var canvas = new BorderedCanvas();
            var child = new Canvas();
            canvas.Children.Add(child);
            Assert.AreEqual(1, ((Canvas)canvas).Children.Count);
            Assert.AreSame(child, canvas.Children[0]);
            Assert.AreSame(child, ((Canvas)canvas).Children[0]);
            canvas.CornerRadius = new CornerRadius(10);
            Assert.AreEqual(2, ((Canvas)canvas).Children.Count);
            Assert.AreSame(child, canvas.Children[0]);
            Assert.AreSame(child, ((Canvas)canvas).Children[1]);
        }

        [UITestMethod]
        public void BorderedCanvas_IsNotReadOnly()
        {
            var canvas = new BorderedCanvas();
            Assert.IsFalse(canvas.Children.IsReadOnly);
        }

        [UITestMethod]
        public void BorderedCanvas_DoesNotContainBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var border = (Border)((Canvas)canvas).Children[0];
            Assert.IsFalse(canvas.Children.Contains(border));
        }

        [UITestMethod]
        public void BorderedCanvas_IndexOfBorderLessThanZero()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var border = (Border)((Canvas)canvas).Children[0];
            Assert.IsTrue(canvas.Children.IndexOf(border) < 0);
        }

        [UITestMethod]
        public void BorderedCanvas_DoesNotContainNonChild()
        {
            var canvas = new BorderedCanvas();
            Assert.IsFalse(canvas.Children.Contains(new Canvas()));
        }

        [UITestMethod]
        public void BorderedCanvas_IndexOfNonChildLessThanZero()
        {
            var canvas = new BorderedCanvas();
            Assert.IsTrue(canvas.Children.IndexOf(new Canvas()) < 0);
        }

        [UITestMethod]
        public void BorderedCanvas_ContainsChild()
        {
            var canvas = new BorderedCanvas();
            var child = new Canvas();
            canvas.Children.Add(child);
            Assert.IsTrue(canvas.Children.Contains(child));
        }

        [UITestMethod]
        public void BorderedCanvas_IndexOfChildBeforeBorder()
        {
            var canvas = new BorderedCanvas();
            var child = new Canvas();
            canvas.Children.Add(child);
            Assert.AreEqual(0, canvas.Children.IndexOf(child));
            canvas.CornerRadius = new CornerRadius(10);
            Assert.AreEqual(0, canvas.Children.IndexOf(child));
        }

        [UITestMethod]
        public void BorderedCanvas_IndexOfChildAfterBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var child = new Canvas();
            canvas.Children.Add(child);
            Assert.AreEqual(0, canvas.Children.IndexOf(child));
        }

        [UITestMethod]
        public void BorderedCanvas_CopyToWithoutBorder()
        {
            var canvas = new BorderedCanvas();
            var c1 = new Canvas();
            var c2 = new Canvas();
            var c3 = new Canvas();
            canvas.Children.Add(c1);
            canvas.Children.Add(c2);
            canvas.Children.Add(c3);
            var arr = new UIElement[4];
            canvas.Children.CopyTo(arr, 1);
            Assert.AreSame(c1, arr[1]);
            Assert.AreSame(c2, arr[2]);
            Assert.AreSame(c3, arr[3]);
        }

        [UITestMethod]
        public void BorderedCanvas_CopyToWithBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var child = new Canvas();
            canvas.Children.Add(child);
            var arr = new UIElement[1];
            canvas.Children.CopyTo(arr, 0);
            Assert.AreSame(child, arr[0]);
        }

        [UITestMethod]
        public void BorderedCanvas_InsertWithoutBorder()
        {
            var canvas = new BorderedCanvas();
            var c1 = new Canvas();
            var c2 = new Canvas();
            canvas.Children.Insert(0, c1);
            canvas.Children.Insert(0, c2);
            Assert.AreSame(c1, ((Canvas)canvas).Children[1]);
            Assert.AreSame(c2, ((Canvas)canvas).Children[0]);
        }

        [UITestMethod]
        public void BorderedCanvas_InsertWithBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var child = new Canvas();
            canvas.Children.Insert(0, child);
            Assert.AreSame(child, ((Canvas)canvas).Children[1]);
        }

        [UITestMethod]
        public void BorderedCanvas_RemoveWithoutBorder()
        {
            var canvas = new BorderedCanvas();
            var child = new Canvas();
            canvas.Children.Add(child);
            Assert.IsTrue(canvas.Children.Remove(child));
        }

        [UITestMethod]
        public void BorderedCanvas_RemoveWithBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var child = new Canvas();
            canvas.Children.Add(child);
            Assert.IsTrue(canvas.Children.Remove(child));
        }

        [UITestMethod]
        public void BorderedCanvas_DoesNotRemoveBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var border = ((Canvas)canvas).Children[0];
            Assert.IsFalse(canvas.Children.Remove(border));
        }

        [UITestMethod]
        public void BorderedCanvas_RemoveNonChild()
        {
            var canvas = new BorderedCanvas();
            Assert.IsFalse(canvas.Children.Remove(new Canvas()));
        }

        [UITestMethod]
        public void BorderedCanvas_RemoveNonChildAfterBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            Assert.IsFalse(canvas.Children.Remove(new Canvas()));
        }

        [UITestMethod]
        public void BorderedCanvas_RemoveAtWithoutBorder()
        {
            var canvas = new BorderedCanvas();
            var child = new Canvas();
            canvas.Children.Add(child);
            canvas.Children.RemoveAt(0);
            Assert.IsFalse(canvas.Children.Contains(child));
        }

        [UITestMethod]
        public void BorderedCanvas_RemoveAtAfterBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var child = new Canvas();
            canvas.Children.Add(child);
            canvas.Children.RemoveAt(0);
            Assert.IsFalse(canvas.Children.Contains(child));
            Assert.AreEqual(1, ((Canvas)canvas).Children.Count);
        }

        [UITestMethod]
        public void BorderedCanvas_ClearWithoutBorder()
        {
            var canvas = new BorderedCanvas();
            var child = new Canvas();
            canvas.Children.Add(child);
            canvas.Children.Clear();
            Assert.AreEqual(0, ((Canvas)canvas).Children.Count);
        }

        [UITestMethod]
        public void BorderedCanvas_ClearAfterBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var child = new Canvas();
            canvas.Children.Add(child);
            canvas.Children.Clear();
            Assert.AreEqual(1, ((Canvas)canvas).Children.Count);
        }

        [UITestMethod]
        public void BorderedCanvas_GetEnumerator()
        {
            var canvas = new BorderedCanvas();
            var c1 = new Canvas();
            var c2 = new Canvas();
            canvas.Children.Add(c1);
            canvas.Children.Add(c2);
            var e = canvas.Children.GetEnumerator();
            Assert.IsTrue(e.MoveNext());
            Assert.AreSame(c1, e.Current);
            Assert.IsTrue(e.MoveNext());
            Assert.AreSame(c2, e.Current);
            Assert.IsFalse(e.MoveNext());
        }

        [UITestMethod]
        public void BorderedCanvas_GetEnumeratorAfterBorder()
        {
            var canvas = new BorderedCanvas();
            canvas.CornerRadius = new CornerRadius(10);
            var c1 = new Canvas();
            var c2 = new Canvas();
            canvas.Children.Add(c1);
            canvas.Children.Add(c2);
            var e = canvas.Children.GetEnumerator();
            Assert.IsTrue(e.MoveNext());
            Assert.AreSame(c1, e.Current);
            Assert.IsTrue(e.MoveNext());
            Assert.AreSame(c2, e.Current);
            Assert.IsFalse(e.MoveNext());
        }
    }
}
