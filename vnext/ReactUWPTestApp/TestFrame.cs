// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace ReactUWPTestApp
{
  public sealed class TestFrame : Frame
  {
    private Type _mainPageType = null;
    private Button _backButton = null;
    private Button _homeButton = null;
    private TextBlock _currentPageTextBlock = null;

    public TestFrame(Type mainPageType)
    {
      this.DefaultStyleKey = typeof(TestFrame);
      _mainPageType = mainPageType;
    }

    protected override void OnApplyTemplate()
    {
      base.OnApplyTemplate();

      this.Navigated += TestFrame_Navigated;

      // NOTE: The "BackButton" element automatically has a handler hooked up to it by Frame
      // just by being named "BackButton"
      _backButton = (Button)GetTemplateChild("BackButton");
      _currentPageTextBlock = (TextBlock)GetTemplateChild("CurrentPageTextBlock");
      _currentPageTextBlock.Text = "Home";

      _homeButton = (Button)GetTemplateChild("HomeButton");
      _homeButton.Click += HomeButton_Click;

    }

    private void TestFrame_Navigated(object sender, NavigationEventArgs e)
    {
      if (e.SourcePageType == _mainPageType)
      {
        _backButton.Visibility = Visibility.Collapsed;
        _currentPageTextBlock.Text = "Home";
      }
      else
      {
        _backButton.Visibility = Visibility.Visible;
        var  testSeclaration = e.Parameter as TestDeclaration;
        _currentPageTextBlock.Text = testSeclaration?.ToString() ?? "";
      }

      GC.Collect();
      GC.WaitForPendingFinalizers();
      GC.Collect();
    }

    private void HomeButton_Click(object sender, RoutedEventArgs e)
    {
      if (CanGoBack)
      {
        GoBack();
      }
    }
  }
}
