using ReactNative.UIManager;
using System;
using Windows.UI.Xaml.Documents;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// View manager for raw text nodes, i.e., <see cref="Run"/> instances.
    /// </summary>
    public class ReactRunManager : ReactTextInlineViewManager<Run, ReactRunShadowNode>
    {
        /// <summary>
        /// The name of this view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTRawText";
            }
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
        public override ReactRunShadowNode CreateShadowNodeInstance()
        {
            return new ReactRunShadowNode();
        }

        /// <summary>
        /// Gets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>Does not return.</returns>
        /// <remarks>
        /// Dimensions are not available for virtual text nodes.
        /// </remarks>
        public override Dimensions GetDimensions(Run view)
        {
            throw new InvalidOperationException(
                $"Dimensions are not available for a virtual text node.");
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The output buffer.</param>
        /// <remarks>
        /// This is a no-op as all layout is performed by the parent text block.
        /// </remarks>
        public override void SetDimensions(Run view, Dimensions dimensions)
        {
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Run"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Run CreateViewInstance(ThemedReactContext reactContext)
        {
            return new Run();
        }
    }
}
