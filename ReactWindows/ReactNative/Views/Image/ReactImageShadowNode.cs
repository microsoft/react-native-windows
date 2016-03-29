using ReactNative.UIManager;
using System;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The shadow node implementation for Image views.
    /// </summary>
    public class ReactImageShadowNode : LayoutShadowNode
    {
        private string _resizeMode;
        private string _source;

        /// <summary>
        /// Set the scaling mode of the image.
        /// </summary>
        /// <param name="resizeMode">The scaling mode.</param>
        [ReactProperty("resizeMode")]
        public void SetResizeMode(string resizeMode)
        {
            _resizeMode = resizeMode;
        }

        /// <summary>
        /// Set the source URI of the image.
        /// </summary>
        /// <param name="source">The source URI.</param>
        [ReactProperty("src")]
        public void SetSource(string source)
        {
            _source = source;
        }

        /// <summary>
        /// Called to aggregate resizeMode and source.
        /// </summary>
        /// <param name="uiViewOperationQueue">The UI operation queue.</param>
        public override void OnCollectExtraUpdates(UIViewOperationQueue uiViewOperationQueue)
        {
            base.OnCollectExtraUpdates(uiViewOperationQueue);

            uiViewOperationQueue.EnqueueUpdateExtraData(ReactTag, Tuple.Create(_resizeMode, _source));
        }
    }
}
