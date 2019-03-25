// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Windows;
using System.Windows.Controls;

namespace ReactNative.DevSupport
{
    sealed partial class DevOptionDialog
    {
        #region Constructor(s)

        /// <summary>
        /// Initializes the DevOptionsDialog
        /// </summary>
        public DevOptionDialog()
        {
            InitializeComponent();
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Add button to dialog
        /// </summary>
        /// <param name="name">Displayed name of button</param>
        /// <param name="onSelect">Action when button is pressed</param>
        public void Add(string name, Action onSelect)
        {
            if (this.OptionsStackPanel != null)
            {
                var button = new Button
                {
                    Content = name,
                };

                button.Click += (sender, args) => onSelect();

                this.OptionsStackPanel.Children.Add(button);
            }
        }

        #endregion

        #region Event Handlers

        private void OnCancelButtonClicked(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        #endregion
    }
}
