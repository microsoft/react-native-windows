using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;
using static System.FormattableString;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// A virtual view manager for raw text nodes.
    /// </summary>
    public class ReactSpanViewManager : ReactTextInlineViewManager<Span, ReactSpanShadowNode>, IViewParentManager
    {
        private const double DefaultFontSize = 15;

        /// <summary>
        /// The view manager name.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTVirtualText";
            }
        }

        /// <summary>
        /// This view does not layout its children.
        /// </summary>
        public bool NeedsCustomLayoutForChildren
        {
            get
            {
                return false;
            }
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(Span view, uint? color)
        {
            view.Foreground = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public void AddView(DependencyObject parent, DependencyObject child, int index)
        {
            var span = (Span)parent;
            var inline = (Inline)child;
            span.Inlines.Insert(index, inline);
        }

        /// <summary>
        /// This method should return the subclass of <see cref="ReactShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view. 
        /// </summary>
        /// <remarks>
        /// In most cases, this will just return an instance of
        /// <see cref="ReactShadowNode"/>.
        /// </remarks>
        /// <returns>The shadow node instance.</returns>
        public override ReactSpanShadowNode CreateShadowNodeInstance()
        {
            return new ReactSpanShadowNode();
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public DependencyObject GetChildAt(DependencyObject parent, int index)
        {
            var span = (Span)parent;
            return span.Inlines[index];
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public int GetChildCount(DependencyObject parent)
        {
            var span = (Span)parent;
            return span.Inlines.Count;
        }

        /// <summary>
        /// Gets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>Does not return.</returns>
        /// <remarks>
        /// Dimensions are not available for virtual text nodes.
        /// </remarks>
        public override Dimensions GetDimensions(Span view)
        {
            throw new InvalidOperationException(
                Invariant($"Dimensions are not available for a virtual text node."));
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public void RemoveAllChildren(DependencyObject parent)
        {
            var span = (Span)parent;
            span.Inlines.Clear();
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public void RemoveChildAt(DependencyObject parent, int index)
        {
            var span = (Span)parent;
            span.Inlines.RemoveAt(index);
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The output buffer.</param>
        /// <remarks>
        /// This is a no-op as all layout is performed by the parent text block.
        /// </remarks>
        public override void SetDimensions(Span view, Dimensions dimensions)
        {
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Span"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Span CreateViewInstance(ThemedReactContext reactContext)
        {
            return new Span();
        }
    }
}
