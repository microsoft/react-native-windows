using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Facebook.CSSLayout;
using ReactNative.UIManager;

namespace ReactNative.Views.Progressbar
{
    public class ReactProgressBarShadowNode : LayoutShadowNode
    {
        public ReactProgressBarShadowNode()
        {
            MeasureFunction = MeasureProgressBar;
        }

        private static MeasureOutput MeasureProgressBar(CSSNode node, float width, float height)
        {
            // TODO: figure out how to properly measure the progressbar.
            return new MeasureOutput(width, 10);
        }
    }
}
