using ReactNative;
using System.ComponentModel;

namespace Playground.Net46
{
    public sealed class ShellViewModel : INotifyPropertyChanged
    {
        #region Constructor(s)

        public ShellViewModel()
        {
        }

        public ShellViewModel(ReactPage rootPage)
        {
            this.rootPage = rootPage;
        }

        #endregion

        #region IShellViewModel Members

        #region Properties

        private ReactPage rootPage = null;

        public ReactPage RootPage
        {
            get
            {
                return this.rootPage;
            }

            set
            {
                this.rootPage = value;
                NotifyPropertyChanged(nameof(RootPage));
            }
        }

        #endregion

        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }

        #endregion
    }
}
