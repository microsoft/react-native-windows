// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Windows;

namespace ReactNative.DevSupport
{
    /// <summary>
    /// The content dialog for red box exception display.
    /// </summary>
    sealed partial class RedBoxDialog : Window
    {
        #region Private Fields

        private readonly Action _onClick;

        #endregion

        #region Properties

        /// <summary>
        /// The error cookie.
        /// </summary>
        public int ErrorCookie { get; set; }

        /// <summary>
        /// The message.
        /// </summary>
        public string Message { get; set; }

        /// <summary>
        /// The stack trace.
        /// </summary>
        public IList<IStackFrame> StackTrace { get; set; } = new List<IStackFrame>();

        #endregion

        #region Constructor(s)

        /// <summary>
        /// Instantiates the <see cref="RedBoxDialog"/>.
        /// </summary>
        /// <param name="onClick">
        /// Action to take when primary button is clicked.
        /// </param>
        public RedBoxDialog(Action onClick)
        {
            InitializeComponent();

            this.DataContext = this;

            _onClick = onClick;
        }

        #endregion

        #region Event Handlers

        private void OnReloadButtonClicked(object sender, RoutedEventArgs e)
        {
            if (_onClick != null)
            {
                _onClick();
            }
            else
            {
                this.Close();
            }
        }

        private void OnCloseButtonClicked(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        #endregion
    }
}
