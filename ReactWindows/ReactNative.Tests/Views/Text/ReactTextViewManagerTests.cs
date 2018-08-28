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
            Assert.AreEqual(text, "test");
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
            Assert.AreEqual(text, "Four");
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
