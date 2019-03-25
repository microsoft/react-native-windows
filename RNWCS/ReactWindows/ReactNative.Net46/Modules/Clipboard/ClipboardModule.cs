// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System;

namespace ReactNative.Modules.Clipboard
{
    /// <summary>
    /// A module that allows JS to get/set clipboard contents.
    /// </summary>
    class ClipboardModule : NativeModuleBase
    {
        private readonly IClipboardInstance _clipboard;
        
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

            DispatcherHelpers.RunOnDispatcher(() =>
            {
                if (_clipboard.ContainsText())
                {
                    var text = _clipboard.GetText();
                    promise.Resolve(text);
                }
                else
                {
                    promise.Resolve("");
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
            DispatcherHelpers.RunOnDispatcher(new Action(() =>
            {
                _clipboard.SetText(text);
            }));
        }
    }
}
