using ReactNative;
using System.Windows;

namespace Playground.Net46
{
    public partial class MainWindow : Window
    {
        #region Constructor(s)

        public MainWindow(ShellViewModel viewModel)
        {
            InitializeComponent();

            this.DataContext = viewModel;
        }

        #endregion
    }
}
