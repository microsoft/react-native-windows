// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using UTFAC = Microsoft.VisualStudio.TestPlatform.UnitTestFramework.AppContainer;
using ReactNative.Views.Text;
using System.Linq;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;

namespace ReactNative.Tests.Views.Text
{
    [TestClass]
    public class ReactTextViewManagerTests
    {
        [UTFAC.UITestMethod]
        public void ReactTextViewManager_Selection_PlainText()
        {
            Run run = new Run();
            run.Text = "Sample test text.";
            RichTextBlock rtb = CreateWithInlines(new Inline[] {run});
            Paragraph paragraph = rtb.Blocks.First() as Paragraph;
            TextPointer start = paragraph.ContentStart.GetPositionAtOffset(8, LogicalDirection.Forward);
            TextPointer end = paragraph.ContentStart.GetPositionAtOffset(12, LogicalDirection.Forward);
            var text = ReactTextViewManager.GetStringByStartAndEndPointers(rtb, start, end);
            Assert.AreEqual(text, @"test");
        }

        [UTFAC.UITestMethod]
        public void ReactTextViewManager_Selection_SpanText()
        {
            // <Span><Run>Sample test text.</Run><Run>Second text.</Run></Span><Run>Third text.</Run><Span><Run>Fourth text.</Run></Span>
            Span span = new Span();
            Run run = new Run();
            run.Text = "Sample test text.";
            span.Inlines.Add(run);
            Run run1 = new Run();
            run1.Text = "Second text.";
            span.Inlines.Add(run1);
            Run run2 = new Run();
            run2.Text = "Third text.";
            Span span1 = new Span();
            Run run3 = new Run();
            run3.Text = "Fourth text.";
            span1.Inlines.Add(run3);

            RichTextBlock rtb = CreateWithInlines(new Inline[] { span, run2, span1 });
            Paragraph paragraph = rtb.Blocks.First() as Paragraph;
            TextPointer start = paragraph.ContentStart.GetPositionAtOffset(50, LogicalDirection.Forward);
            TextPointer end = paragraph.ContentStart.GetPositionAtOffset(54, LogicalDirection.Forward);
            var text = ReactTextViewManager.GetStringByStartAndEndPointers(rtb, start, end);
            Assert.AreEqual(text, @"Four");
        }

        [UTFAC.UITestMethod]
        public void ReactTextViewManager_Selection_BoldUnderlineText()
        {
            // <Bold><Run>Sample test text.</Run><Run>Second text.</Run></Bold><Run>Third text.</Run><Underline><Run>Fourth text.</Run></Underline>
            Bold bold = new Bold();
            Run run = new Run();
            run.Text = "Sample test text.";
            bold.Inlines.Add(run);
            Run run1 = new Run();
            run1.Text = "Second text.";
            bold.Inlines.Add(run1);
            Run run2 = new Run();
            run2.Text = "Third text.";
            Underline underline = new Underline();
            Run run3 = new Run();
            run3.Text = "Fourth text.";
            underline.Inlines.Add(run3);

            RichTextBlock rtb = CreateWithInlines(new Inline[] { bold, run2, underline });
            Paragraph paragraph = rtb.Blocks.First() as Paragraph;
            TextPointer start = paragraph.ContentStart.GetPositionAtOffset(50, LogicalDirection.Forward);
            TextPointer end = paragraph.ContentStart.GetPositionAtOffset(54, LogicalDirection.Forward);
            var text = ReactTextViewManager.GetStringByStartAndEndPointers(rtb, start, end);
            Assert.AreEqual(text, @"Four");
        }

        [UTFAC.UITestMethod]
        public void ReactTextViewManager_Selection_HyperlinkText()
        {
            // <Span><Run>Sample test text.</Run><Run>Second text.</Run></Span><Run>Third text.</Run><Span><Hyperlink NavigateUri='http://www.test.com'>HyperlinkTest.</Hyperlink></Span>
            Span span = new Span();
            Run run = new Run();
            run.Text = "Sample test text.";
            span.Inlines.Add(run);
            Run run1 = new Run();
            run1.Text = "Second text.";
            span.Inlines.Add(run1);
            Run run2 = new Run();
            run2.Text = "Third text.";
            Span span1 = new Span();
            Hyperlink hyperlink = new Hyperlink();
            hyperlink.NavigateUri = new System.Uri("http://www.test.com");
            span1.Inlines.Add(hyperlink);

            RichTextBlock rtb = CreateWithInlines(new Inline[] { span, run2, span1 });
            Paragraph paragraph = rtb.Blocks.First() as Paragraph;
            TextPointer start = paragraph.ContentStart.GetPositionAtOffset(49, LogicalDirection.Forward);
            TextPointer end = paragraph.ContentStart.GetPositionAtOffset(50, LogicalDirection.Forward);
            var text = ReactTextViewManager.GetStringByStartAndEndPointers(rtb, start, end);
            Assert.AreEqual(text, @"http://www.test.com/");
        }

        [UTFAC.UITestMethod]
        public void ReactTextViewManager_Selection_InlineUIContainer()
        {
            // <Span><Run>Sample test text.</Run><Run>Second text.</Run></Span><Run>Third text.</Run><InlineUIContainer><TextBlock Text='InlineUIContainerText'/></InlineUIContainer>
            Span span = new Span();
            Run run = new Run();
            run.Text = "Sample test text.";
            span.Inlines.Add(run);
            Run run1 = new Run();
            run1.Text = "Second text.";
            span.Inlines.Add(run1);
            Run run2 = new Run();
            run2.Text = "Third text.";
            InlineUIContainer inlineUIContainer = new InlineUIContainer();
            TextBlock tb = new TextBlock();
            tb.Text = "InlineUIContainerText";
            inlineUIContainer.Child = tb;

            RichTextBlock rtb = CreateWithInlines(new Inline[] { span, run2, inlineUIContainer });
            Paragraph paragraph = rtb.Blocks.First() as Paragraph;
            TextPointer start = paragraph.ContentStart.GetPositionAtOffset(49, LogicalDirection.Forward);
            TextPointer end = paragraph.ContentStart.GetPositionAtOffset(50, LogicalDirection.Forward);
            var text = ReactTextViewManager.GetStringByStartAndEndPointers(rtb, start, end);
            Assert.AreEqual(text, @"InlineUIContainerText");
        }

        [UTFAC.UITestMethod]
        public void ReactTextViewManager_Selection_EmbeddedSpan()
        {
            // <Span><Run>@</Run><Span><Run>First text.</Run></Span><Run>Third text.</Run></Span>
            Span span = new Span();
            Run run = new Run();
            run.Text = "@";
            span.Inlines.Add(run);
            Span span1 = new Span();
            Run run1 = new Run();
            run1.Text = "First text.";
            span1.Inlines.Add(run1);
            span.Inlines.Add(span1);
            Run run2 = new Run();
            run2.Text = "Third text.";
            span.Inlines.Add(run2);

            RichTextBlock rtb = CreateWithInlines(new Inline[] { span });
            Paragraph paragraph = rtb.Blocks.First() as Paragraph;
            TextPointer start = paragraph.ContentStart.GetPositionAtOffset(7, LogicalDirection.Forward);
            TextPointer end = paragraph.ContentStart.GetPositionAtOffset(8, LogicalDirection.Forward);
            var text = ReactTextViewManager.GetStringByStartAndEndPointers(rtb, start, end);
            Assert.AreEqual(text, @"i");
        }

        private RichTextBlock CreateWithInlines(Inline[] inlines)
        {
            RichTextBlock rtb = new RichTextBlock();
            Paragraph par = new Paragraph();
            foreach (Inline inline in inlines)
            {
                par.Inlines.Add(inline);
            }
            rtb.Blocks.Add(par);
            return rtb;
        }
    }
}
