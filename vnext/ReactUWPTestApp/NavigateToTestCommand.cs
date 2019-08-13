using System;
using System.Windows.Input;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ReactUWPTestApp
{
  public class NavigateToTestCommand : ICommand
  {
    public event EventHandler CanExecuteChanged;

    public Frame Frame { get; set; }

    public NavigateToTestCommand()
    {
      if (CanExecuteChanged != null)
      {
        CanExecuteChanged(this, null);
      }
    }

    public bool CanExecute(object parameter)
    {
      return (parameter != null && parameter is TestDeclaration);
    }

    public void Execute(object parameter)
    {
      var testDeclaration = parameter as TestDeclaration;
      var rootFrame = Frame ?? Window.Current.Content as Frame;
      rootFrame.Navigate(testDeclaration.PageType, testDeclaration);
    }
  }
}
