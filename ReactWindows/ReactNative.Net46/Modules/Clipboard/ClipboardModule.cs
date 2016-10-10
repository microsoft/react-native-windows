using ReactNative.Bridge;
using System;
using System.Threading.Tasks;
using AsyncClipboardService.Clipboard;
using AsyncWindowsClipboard;
using System.Threading;
using System.Windows.Threading;
// using DataTransfer = Windows.ApplicationModel.DataTransfer;

namespace ReactNative.Modules.Clipboard
{
    /// <summary>
    /// A module that allows JS to get/set clipboard contents.
    /// </summary>
    class ClipboardModule : NativeModuleBase
    {
        private readonly IClipboardInstance _clipboard;
        private WindowsClipboardService _clipboardService = new WindowsClipboardService();

        public ClipboardModule() : this(new ClipboardInstance())
        {

        }

        public ClipboardModule(IClipboardInstance clipboard)
        {
            _clipboard = clipboard;
        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "Clipboard";
            }
        }

        /// <summary>
        /// Get the clipboard content through a promise.
        /// </summary>
        /// <param name="promise">The promise.</param>
        [ReactMethod]
        public void getString(IPromise promise)
        {
            if (promise == null)
            {
                throw new ArgumentNullException(nameof(promise));
            }

            RunOnDispatcher(async () =>
            {
                try
                {
                    if (_clipboard.GetContent() == null)
                    {
                        promise.Resolve("");
                    }
                    else if (_clipboard.ContainsText())
                    {
                        var text = await _clipboardService.GetTextAsync().ConfigureAwait(false);
                        promise.Resolve(text);
                    }
                    else
                    {
                        promise.Resolve("");
                    }
                }
                catch (Exception ex)
                {
                    promise.Reject(ex);
                }
            });
        }

        /// <summary>
        /// Add text to the clipboard or clear the clipboard.
        /// </summary>
        /// <param name="text">The text. If null clear clipboard.</param>
        [ReactMethod]
        public void setString(string text)
        {
            RunOnDispatcher(async () =>
            {
                if (text == null)
                {
                    _clipboard.Clear();
                }
                else
                {
                    await _clipboardService.SetTextAsync(text);
                }
            });
        }

        /// <summary>
        /// Run action on UI thread.
        /// </summary>
        /// <param name="action">The action.</param>
        private static async void RunOnDispatcher(Func<Task> action)
        {
            await Dispatcher.CurrentDispatcher.InvokeAsync(action).Task.ConfigureAwait(false);
        }
    }
}
