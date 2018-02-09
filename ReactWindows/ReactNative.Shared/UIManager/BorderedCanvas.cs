using System;

#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Represents a Canvas with an optional Border inside.
    /// </summary>
    public class BorderedCanvas : Canvas
    {
        private Border _border = null;

        /// <summary>
        /// The Border associated with this Canvas or null if it doesn't have a border.
        /// The Border is always the first element in the Children collection.
        /// </summary>
        public Border Border
        {
            get
            {
                return _border;
            }
            set
            {
                if (_border != null)
                {
                    throw new InvalidOperationException("The Canvas already has a Border");
                }

                _border = value;

                Children.Insert(0, _border);
            }
        }
    }
}
