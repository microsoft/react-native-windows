// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The view manager for text views.
    /// </summary>
    public class ReactSimpleTextViewManager : BaseViewManager<TextBlock, ReactSimpleTextShadowNode>
    {
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTSimpleText";
            }
        }

        /// <summary>
        /// Creates the shadow node instance.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactSimpleTextShadowNode CreateShadowNodeInstance()
        {
            return new ReactSimpleTextShadowNode();
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(TextBlock view, uint? color)
        {
            view.Foreground = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Sets whether or not the text is selectable.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="selectable">A flag indicating whether or not the text is selectable.</param>
        [ReactProp("selectable")]
        public void SetSelectable(TextBlock view, bool selectable)
        {
            view.IsTextSelectionEnabled = selectable;
        }


        /// <summary>
        /// Receive extra updates from the shadow node.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(TextBlock root, object extraData)
        {
            var textNode = extraData as ReactSimpleTextShadowNode;
            if (textNode != null)
            {
                textNode.UpdateTextBlock(root);
            }
        }

        /// <summary>
        /// Creates the view instance.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override TextBlock CreateViewInstance(ThemedReactContext reactContext)
        {
            var textBlock = new TextBlock
            {
                IsTextSelectionEnabled = false,
                TextAlignment = TextAlignment.DetectFromContent,
                TextTrimming = TextTrimming.CharacterEllipsis,
                TextWrapping = TextWrapping.Wrap,
            };

            return textBlock;
        }
    }
}
