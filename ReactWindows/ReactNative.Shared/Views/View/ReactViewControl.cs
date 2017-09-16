using System;

#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.Views.View
{
    /// <summary>
    /// Native control for React view.
    /// </summary>
    public class ReactViewControl : UserControl
    {
        private readonly Canvas _canvas;

        private Border _border = null;

        /// <summary>
        /// Instantiates the <see cref="ReactViewControl"/>. 
        /// </summary>
        public ReactViewControl()
        {
            Content = _canvas = new Canvas
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                VerticalAlignment = VerticalAlignment.Stretch,
            };
        }

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

        /// <summary>
        /// The view children.
        /// </summary>
        public UIElementCollection Children
        {
            get
            {
                return _canvas.Children;
            }
        }
    }
}
