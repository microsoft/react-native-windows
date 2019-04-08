// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reactive.Disposables;
using System.Threading;
using System.Windows;

namespace ReactNative.DevSupport
{
    /// <summary>
    /// Content dialog for when the app is waiting.
    /// </summary>
    /// <remarks>
    /// This is used when awaiting the regeneration of the JavaScript bundle.
    /// </remarks>
    sealed partial class ProgressDialog : Window, IDisposable
    {
        #region Private Fields

        private readonly CancellationDisposable _cancellationDisposable;

        #endregion

        #region Properties

        /// <summary>
        /// The title of the dialog.
        /// </summary>
        public string Heading { get; }

        /// <summary>
        /// The message displayed in the dialog.
        /// </summary>
        public string Message { get; }

        /// <summary>
        /// The cancellation token cancelled upon dialog dismissal.
        /// </summary>
        public CancellationToken Token => _cancellationDisposable.Token;

        #endregion

        #region Constructor(s)

        /// <summary>
        /// Instantiates the <see cref="ProgressDialog"/>.
        /// </summary>
        /// <param name="title">The title.</param>
        /// <param name="message">The message.</param>
        public ProgressDialog(string title, string message)
        {
            Heading = title;
            Message = message;

            InitializeComponent();

            DataContext = this;

            _cancellationDisposable = new CancellationDisposable();
        }

        #endregion

        #region Event Handlers

        private void OnDismissButtonClicked(object sender, RoutedEventArgs e)
        {
            _cancellationDisposable.Dispose();
        }

        #endregion

        #region IDisposable Members

        /// <summary>
        /// Disposes the <see cref="ProgressDialog"/>. 
        /// </summary>
        public void Dispose()
        {
            _cancellationDisposable.Dispose();
        }

        #endregion
    }
}
