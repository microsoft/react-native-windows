using System;
using System.Reactive.Disposables;
using System.Threading;
using Windows.UI.Xaml.Controls;

namespace ReactNative.DevSupport
{
    /// <summary>
    /// Content dialog for when the app is waiting.
    /// </summary>
    /// <remarks>
    /// This is used when awaiting the regeneration of the JavaScript bundle.
    /// </remarks>
    sealed partial class ProgressDialog : ContentDialog, IDisposable
    {
        private readonly CancellationDisposable _cancellationDisposable;

        /// <summary>
        /// Instantiates the <see cref="ProgressDialog"/>.
        /// </summary>
        /// <param name="title">The title.</param>
        /// <param name="message">The message.</param>
        public ProgressDialog(string title, string message)
        {
            this.InitializeComponent();

            Heading = title;
            Message = message;

            _cancellationDisposable = new CancellationDisposable();
        }

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
        public CancellationToken Token
        {
            get
            {
                return _cancellationDisposable.Token;
            }
        }

        /// <summary>
        /// Disposes the <see cref="ProgressDialog"/>. 
        /// </summary>
        public void Dispose()
        {
            _cancellationDisposable.Dispose();
        }

        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
            _cancellationDisposable.Dispose();
        }
    }
}
