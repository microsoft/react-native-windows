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
