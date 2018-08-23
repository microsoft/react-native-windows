// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Accessibility;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System.Diagnostics;
using System.Linq;
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The view manager for text views.
    /// </summary>
    public class ReactTextViewManager : ViewParentManager<RichTextBlock, ReactTextShadowNode>
    {
        private static readonly IReactCompoundView s_compoundView = new ReactTextCompoundView();

        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTText";
            }
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(RichTextBlock view, uint? color)
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
        public void SetSelectable(RichTextBlock view, bool selectable)
        {
            view.IsTextSelectionEnabled = selectable;
        }

        /// <summary>
        /// Sets <see cref="ImportantForAccessibility"/> for the RichTextBlock.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="importantForAccessibilityValue">The string to be parsed as <see cref="ImportantForAccessibility"/>.</param>
        [ReactProp(ViewProps.ImportantForAccessibility)]
        public void SetImportantForAccessibility(RichTextBlock view, string importantForAccessibilityValue)
        {
            var importantForAccessibility = EnumHelpers.ParseNullable<ImportantForAccessibility>(importantForAccessibilityValue) ?? ImportantForAccessibility.Auto;
            AccessibilityHelper.SetImportantForAccessibility(view, importantForAccessibility);
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(RichTextBlock parent, DependencyObject child, int index)
        {
            var inlineChild = child as Inline;
            if (inlineChild == null)
            {
                inlineChild = new InlineUIContainer
                {
                    Child = (UIElement)child,
                };
            }

            parent.Blocks.OfType<Paragraph>().First().Inlines.Insert(index, inlineChild);
        }

        /// <summary>
        /// Creates the shadow node instance.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactTextShadowNode CreateShadowNodeInstance()
        {
            return new ReactTextShadowNode();
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public override DependencyObject GetChildAt(RichTextBlock parent, int index)
        {
            var child = parent.Blocks.OfType<Paragraph>().First().Inlines[index];
            if (child is InlineUIContainer childInlineContainer)
            {
                return childInlineContainer.Child;
            }
            else
            {
                return child;
            }
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(RichTextBlock parent)
        {
            return parent.Blocks.OfType<Paragraph>().First().Inlines.Count;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(RichTextBlock parent)
        {
            var inlines = parent.Blocks.OfType<Paragraph>().First().Inlines;
            inlines.Clear();
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(RichTextBlock parent, int index)
        {
            var inlines = parent.Blocks.OfType<Paragraph>().First().Inlines;
            inlines.RemoveAt(index);
        }

        /// <summary>
        /// Receive extra updates from the shadow node.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(RichTextBlock root, object extraData)
        {
            base.UpdateExtraData(root, extraData);

            if (extraData is ReactTextShadowNode textNode)
            {
                textNode.UpdateTextBlock(root);
            }

            AccessibilityHelper.OnElementChanged(root);
        }

        /// <summary>
        /// Creates the view instance.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override RichTextBlock CreateViewInstance(ThemedReactContext reactContext)
        {
            var richTextBlock = new RichTextBlock
            {
                IsTextSelectionEnabled = false,
                TextAlignment = TextAlignment.DetectFromContent,
                TextTrimming = TextTrimming.CharacterEllipsis,
                TextWrapping = TextWrapping.Wrap,
            };

            richTextBlock.Blocks.Add(new Paragraph());
            richTextBlock.SetReactCompoundView(s_compoundView);

            return richTextBlock;
        }

        /// <summary>
        /// Installing the event emitters for the RichTextBlock control.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The RichTextBlock view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, RichTextBlock view)
        {
            base.AddEventEmitters(reactContext, view);
            view.SelectionChanged += OnSelectionChanged;
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="ReactTextViewManager"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, RichTextBlock view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.SelectionChanged -= OnSelectionChanged;
        }

        /// <summary>
        /// Called when selection changes for the RichTextBlock.
        /// </summary>
        /// <param name="sender">The RichTextBlock that is the source of the event.</param>
        /// <param name="e">Event arguments.</param>
        private void OnSelectionChanged(object sender, RoutedEventArgs e)
        {
            RichTextBlock view = sender as RichTextBlock;
            string selection = GetSelectedString(view);

            // Fire the event.
            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextBlockSelectionEvent(
                        view.GetTag(),
                        selection));
        }

        /// <summary>
        /// Returns a text representation of the selection in the RichTextBlock.
        /// </summary>
        /// <param name="view">The RichTextBlock.</param>
        /// <returns>Text representation of the selection.</returns>
        private string GetSelectedString(RichTextBlock view)
        {
            TextPointer selectionStart = view.SelectionStart;
            TextPointer selectionEnd = view.SelectionEnd;

            if (selectionStart == null || selectionEnd == null || selectionStart == selectionEnd)
            {
                return null;
            }

            // Parse the RichTextBlock contents.
            StringBuilder selectedText = new StringBuilder();
            Debug.Assert(view.Blocks.Count == 1, "RichTextBlock is expected to contain only one paragraph.");
            Paragraph paragraph = view.Blocks.First() as Paragraph;
            ProcessSelectedInlines(paragraph.Inlines, selectedText, false, selectionStart, selectionEnd);

            return selectedText.ToString();
        }

        /// <summary>
        /// Recursively processes InlineCollection and builds text representation of the RichTextBlock selected content.
        /// </summary>
        /// <param name="inlines">InlineCollection to be processed.</param>
        /// <param name="selectedText">Will contain the text representation upon procedure completion.</param>
        /// <param name="accumulate">If 'true', includes inlines as part of the processed selection.</param>
        /// <param name="selectionStart">TextPointer to selection start.</param>
        /// <param name="selectionEnd">TextPointer to selection end.</param>
        /// <returns>'true' if recursion terminal condition is met.</returns>
        private bool ProcessSelectedInlines(
            InlineCollection inlines,
            StringBuilder selectedText,
            bool accumulate,
            TextPointer selectionStart,
            TextPointer selectionEnd)
        {
            foreach (Inline inline in inlines)
            {
                // Begin building the string from selectionStart.
                if (!accumulate && (object.ReferenceEquals(inline, selectionStart.Parent) || selectionStart.Offset <= inline.ContentEnd.Offset))
                {
                    accumulate = true;
                }

                // Check for terminal condition.
                if (inline.ContentStart.Offset > selectionEnd.Offset)
                {
                    return true;
                }

                switch (inline)
                {
                    case Hyperlink hyperlink:
                        if (hyperlink.NavigateUri != null)
                        {
                            if (accumulate)
                            {
                                selectedText.Append(hyperlink.NavigateUri.ToString());
                            }
                        }
                        else
                        {
                            if (ProcessSelectedInlines(hyperlink.Inlines, selectedText, accumulate, selectionStart, selectionEnd))
                            {
                                return true;
                            }
                        }
                        break;
                    // Also covers Bold, Italic, Underline.
                    case Span span:
                        if (ProcessSelectedInlines(span.Inlines, selectedText, accumulate, selectionStart, selectionEnd))
                        {
                            return true;
                        }
                        break;
                    case Run run:
                        if (accumulate)
                        {
                            int rangeStart = 0;
                            // Check for selection start in this run.
                            if (object.ReferenceEquals(inline, selectionStart.Parent))
                            {
                                rangeStart = selectionStart.Offset - inline.ContentStart.Offset;
                            }
                            // Check for selection end in this run.
                            if (object.ReferenceEquals(inline, selectionEnd.Parent))
                            {
                                int rangeEnd = selectionEnd.Offset - inline.ContentStart.Offset;
                                selectedText.Append(run.Text.Substring(rangeStart, rangeEnd - rangeStart));
                            }
                            else
                            {
                                selectedText.Append(run.Text.Substring(rangeStart));
                            }
                        }
                        break;
                    case LineBreak lineBreak:
                        if (accumulate)
                        {
                            selectedText.AppendLine();
                        }
                        break;
                    case InlineUIContainer inlineUIContainer:
                        if (accumulate)
                        {
                            // Since InlineUIContainer can contain any UIElement as a child, we'll make some assumptions in order to simplify processing.
                            // 1. Entire InlineUIContainer content is considered selected.
                            // 2. If the child is a TextBlock, its Text property returns the entire content properly (since it can't contain InlineUIContainers).
                            // 3. If the InlineUIContainer Child is a Panel, we process only TextBlock children (and not RichTextBlocks) so that we limit recursion.
                            if (inlineUIContainer.Child is TextBlock textBlock)
                            {
                                selectedText.Append(textBlock.Text);
                            }
                            else if (inlineUIContainer.Child is Panel panel)
                            {
                                foreach (UIElement uiElement in panel.Children)
                                {
                                    if (uiElement is TextBlock panelTextBlock)
                                    {
                                        selectedText.Append(panelTextBlock.Text);
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }

                // Check for terminal condition.
                if (object.ReferenceEquals(inline, selectionEnd.Parent))
                {
                    return true;
                }
            }

            return false;
        }
    }
}
