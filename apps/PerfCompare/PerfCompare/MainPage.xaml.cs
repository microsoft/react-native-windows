// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PerfCompare
{
    public partial class MainPage : Page, INotifyPropertyChanged
    {
        public int TotalMessages
        {
            get
            {
                return App.TotalMessages;
            }
            set
            {
                App.TotalMessages = value;
                OnPropertyChanged(nameof(TotalMessages));
            }
        }

        public MainPage()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Frame rootFrame = Window.Current.Content as Frame;

            App.PerfStats.Start();

            if (sender == xamlMarkupBtn)
            {
                rootFrame.Navigate(typeof(MarkupPage));
            }
            else if (sender == rnwcsBtn)
            {
                rootFrame.Navigate(typeof(ReactNativeCurrentPage));
            }
            else if (sender == rnwcppBtn)
            {
                rootFrame.Navigate(typeof(ReactNativeVNextPage));
            }
        }
    }
}
