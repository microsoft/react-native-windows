using System;
using System.Windows;
using System.Windows.Controls;

namespace ReactNative.DevSupport
{
    sealed partial class DevOptionDialog : Window
    {
        public DevOptionDialog()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Add button to dialog
        /// </summary>
        /// <param name="name">Displayed name of button</param>
        /// <param name="onSelect">Action when button is pressed</param>
        public void Add(string name, Action onSelect)
        {
            var button = new Button
            {
                Content = name,
            };

            button.Click += (sender, args) => onSelect();

            OptionsStackPanel.Children.Add(button);
        }
    }
}
