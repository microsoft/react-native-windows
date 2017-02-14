using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using Windows.UI;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The shadow node implementation for Image views.
    /// </summary>
    public class ReactImageShadowNode : LayoutShadowNode
    {
        private Color? _imageBackgroundColor = null;
        private Color? _imageTintColor = null;
        private JArray _imageSources = null;

        /// <summary>
        /// Instantiates the <see cref="ReactImageShadowNode"/>.
        /// </summary>
        public ReactImageShadowNode()
        {
        }

        /// <summary>
        /// Set the color of all the non-transparent pixels to the tintColor.
        /// </summary>
        /// <param name="tintColor">The scaling mode.</param>
        [ReactProp("tintColor")]
        public void SetTintColor(uint? tintColor)
        {
            if (tintColor != null)
            {
                _imageTintColor = ColorHelpers.Parse((uint)tintColor);
            }
            else
            {
                _imageTintColor = null;
            }

            MarkUpdated();
        }

        /// <summary>
        /// Set the color of all the non-transparent pixels to the tintColor.
        /// </summary>
        /// <param name="backgroundColor">The scaling mode.</param>
        [ReactProp("backgroundColor")]
        public void SetBackgroundColor(uint? backgroundColor)
        {
            if (backgroundColor != null)
            {
                _imageBackgroundColor = ColorHelpers.Parse((uint)backgroundColor);
            }
            else
            {
                _imageBackgroundColor = null;
            }

            MarkUpdated();
        }

        /// <summary>
        /// Set the source URI of the image.
        /// </summary>
        /// <param name="sources">The source URI array.</param>
        [ReactProp("src")]
        public void SetSource(JArray sources)
        {
            if (sources != _imageSources)
            {
                _imageSources = sources;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Called to aggregate all the changes to the image nodes.
        /// </summary>
        /// <param name="uiViewOperationQueue">The UI operation queue.</param>
        public override void OnCollectExtraUpdates(UIViewOperationQueue uiViewOperationQueue)
        {
            base.OnCollectExtraUpdates(uiViewOperationQueue);

            var args = Tuple.Create(
                _imageSources,
                _imageTintColor,
                _imageBackgroundColor);

            uiViewOperationQueue.EnqueueUpdateExtraData(ReactTag, args);
        }
    }
}
