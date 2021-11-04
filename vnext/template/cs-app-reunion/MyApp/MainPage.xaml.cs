using Microsoft.ReactNative;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.Linq;

namespace {{ namespace }}
{
  public sealed partial class MainPage : Page
  {
    public static MainPage Current;

    public MainPage()
    {
      InitializeComponent();
      reactRootView.Loaded += ReactRootView_Loaded;
    }

    private async void ReactRootView_Loaded(object sender, RoutedEventArgs e)
    {
      var app = Application.Current as App;
      XamlUIService.SetXamlRoot(app.Host.InstanceSettings.Properties, this.Content.XamlRoot);
      reactRootView.ReactNativeHost = app.Host;
      await app.Host.LoadInstance();
    }
  }
}
