using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace ReactNative.Views.Scroll
{
    /// <summary>
    /// Extended <see cref="System.Windows.Controls.ScrollViewer"/> with partial <see cref="IScrollView"/> interface for unit testing
    /// </summary>
    public class ScrollView : ScrollViewer, IScrollView
    {
    }
}
