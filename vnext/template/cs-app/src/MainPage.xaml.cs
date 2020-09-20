using Microsoft.ReactNative;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using {{ xamlNamespace }};
using {{ xamlNamespace }}.Controls;
using {{ xamlNamespace }}.Controls.Primitives;
using {{ xamlNamespace }}.Data;
using {{ xamlNamespace }}.Input;
using {{ xamlNamespace }}.Media;
using {{ xamlNamespace }}.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace {{ namespace }}
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            var app = Application.Current as App;
            reactRootView.ReactNativeHost = app.Host;
        }
    }
}
