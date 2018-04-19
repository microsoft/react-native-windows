// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using Windows.UI.Xaml;
#if WINDOWS_UWP
using Windows.UI.Xaml.Documents;
#else
using System.Windows.Documents;
#endif

namespace ReactNative.Views.Text
{
    /// <summary>
    /// A view manager class for hyperlink text nodes.
    /// </summary>
    public class ReactHyperlinkViewManager : ReactSpanViewManager
    {
        /// <summary>
        /// The view manager name.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTHyperlink";
            }
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Hyperlink"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Span CreateViewInstance(ThemedReactContext reactContext)
        {
            return new Hyperlink();
        }
    }
}