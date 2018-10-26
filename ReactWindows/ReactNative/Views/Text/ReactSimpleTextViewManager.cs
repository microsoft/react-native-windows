// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Accessibility;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The view manager for text views.
    /// </summary>
    public class ReactSimpleTextViewManager : BaseViewManager<TextBlock, ReactSimpleTextShadowNode>
    {
        private readonly ViewKeyedDictionary<TextBlock, TextBlockData> _textBlockData =
           new ViewKeyedDictionary<TextBlock, TextBlockData>();

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

        ///<summary>
        /// Sets <see cref="ImportantForAccessibility"/> for the TextBlock.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="importantForAccessibilityValue">The string to be parsed as <see cref="ImportantForAccessibility"/>.</param>
        [ReactProp(ViewProps.ImportantForAccessibility)]
        public void SetImportantForAccessibility(TextBlock view, string importantForAccessibilityValue)
        {
            var importantForAccessibility = EnumHelpers.ParseNullable<ImportantForAccessibility>(importantForAccessibilityValue) ?? ImportantForAccessibility.Auto;
            AccessibilityHelper.SetImportantForAccessibility(view, importantForAccessibility);
        }
        
        /// <summary>
        /// Sets whether or not the default context menu should be shown.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="disabled">A flag indicating whether or not the default context menu should be shown.</param>
        [ReactProp("disableContextMenu")]
        public void SetDisableContextMenu(TextBlock view, bool disabled)
        {
            if (disabled)
            {
                var found = _textBlockData.TryGetValue(view, out var data);
                if (!found)
                {
                    data = new TextBlockData() { IsDefaultContextMenuDisabled = false };
                    _textBlockData.AddOrUpdate(view, data);
                }
                if (!data.IsDefaultContextMenuDisabled)
                {
                    view.ContextMenuOpening += OnContextMenuOpening;
                    data.IsDefaultContextMenuDisabled = true;
                }
            }
            else
            {
                _textBlockData.Remove(view);
                view.ContextMenuOpening -= OnContextMenuOpening;
            }
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

        /// <summary>
        /// Installing the event emitters for the TextBlock control.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The TextBlock view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, TextBlock view)
        {
            base.AddEventEmitters(reactContext, view);
            view.SelectionChanged += OnSelectionChanged;
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="ReactSimpleTextViewManager"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, TextBlock view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.SelectionChanged -= OnSelectionChanged;

            _textBlockData.Remove(view);
            view.ContextMenuOpening -= OnContextMenuOpening;
        }

        private void OnSelectionChanged(object sender, RoutedEventArgs e)
        {
            TextBlock view = sender as TextBlock;
            string selection;

            TextPointer selectionStart = view.SelectionStart;
            TextPointer selectionEnd = view.SelectionEnd;
            if (selectionStart == null || selectionEnd == null || selectionStart == selectionEnd)
            {
                selection = null;
            }
            else
            {
                selection = view.SelectedText;
            }

            // Fire the event.
            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextBlockSelectionEvent(
                        view.GetTag(),
                        selection));
        }

        private void OnContextMenuOpening(object sender, ContextMenuEventArgs e)
        {
            // Prevent the opening of the context menu
            e.Handled = true;
        }

        private class TextBlockData
        {
            public bool IsDefaultContextMenuDisabled;
        }
    }
}
