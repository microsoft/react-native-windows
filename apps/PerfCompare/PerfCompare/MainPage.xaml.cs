// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PerfCompare
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
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

            }
            else if (sender == rnwcppBtn)
            {

            }
        }
    }
}
