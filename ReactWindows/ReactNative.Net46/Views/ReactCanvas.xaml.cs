using System.Windows;
using System.Windows.Controls;

namespace ReactNative.Views
{
    public partial class ReactCanvas : UserControl
    {
        #region Dependency Properties

        public ReactPage Source
        {
            get { return (ReactPage)GetValue(SourceProperty); }
            set { SetValue(SourceProperty, value); }
        }

        public static readonly DependencyProperty SourceProperty =
            DependencyProperty.Register("Source", typeof(ReactPage), typeof(ReactCanvas), new PropertyMetadata(null));

        #endregion

        #region Constructor(s)

        public ReactCanvas()
        {
            InitializeComponent();
        }

        #endregion
    }
}
