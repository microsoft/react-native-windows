// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.Web.Events;
using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Windows.Controls;
using System.Windows.Navigation;
using static System.FormattableString;

namespace ReactNative.Views.Web
{
    /// <summary>
    /// A view manager responsible for rendering webview.
    /// </summary>
    public class ReactWebViewManager : SimpleViewManager<WebBrowser>
    {
        private const string BLANK_URL = "about:blank";

        private const int CommandGoBack = 1;
        private const int CommandGoForward = 2;
        private const int CommandReload = 3;

        private readonly Dictionary<int, string> _injectedJS = new Dictionary<int, string>();

        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTWebView";
            }
        }

        /// <summary>
        /// The commands map for the webview manager.
        /// </summary>
        public override JObject ViewCommandsMap
        {
            get
            {
                return new JObject
                {
                    { "goBack", CommandGoBack },
                    { "goForward", CommandGoForward },
                    { "reload", CommandReload },
                };
            }
        }

        /// <summary>
        /// Sets whether JavaScript is enabled or not.
        /// </summary>
        /// <param name="view">A webview instance.</param>
        /// <param name="enabled">A flag signaling whether JavaScript is enabled.</param>
        [ReactProp("javaScriptEnabled")]
        public void SetJavaScriptEnabled(WebBrowser view, bool enabled)
        {
            //view.Settings.IsJavaScriptEnabled = enabled;
        }

        /// <summary>
        /// Sets whether Indexed DB is enabled or not.
        /// </summary>
        /// <param name="view">A webview instance.</param>
        /// <param name="enabled">A flag signaling whether Indexed DB is enabled.</param>
        [ReactProp("indexedDbEnabled")]
        public void SetIndexedDbEnabled(WebBrowser view, bool enabled)
        {
            //view.Settings.IsIndexedDBEnabled = enabled;
        }

        /// <summary>
        /// Sets the JavaScript to be injected when the webpage loads.
        /// </summary>
        /// <param name="view">A webview instance.</param>
        /// <param name="injectedJavaScript">An injected JavaScript.</param>
        [ReactProp("injectedJavaScript")]
        public void SetInjectedJavaScript(WebBrowser view, string injectedJavaScript)
        {
            _injectedJS[view.GetTag()] = injectedJavaScript;
        }

        /// <summary>
        /// Sets webview source.
        /// </summary>
        /// <param name="view">A webview instance.</param>
        /// <param name="source">A source for the webview (either static html or an uri).</param>
        [ReactProp("source")]
        public void SetSource(WebBrowser view, JObject source)
        {
            if (source != null)
            {
                var html = source.Value<string>("html");
                if (html != null)
                {
                    var baseUrl = source.Value<string>("baseUrl");
                    if (baseUrl != null)
                    {
                        view.Source = new Uri(baseUrl);
                    }

                    view.NavigateToString(html);
                    return;
                }

                var uri = source.Value<string>("uri");
                if (uri != null)
                {
                    string previousUri = view.Source?.OriginalString;
                    if (!String.IsNullOrWhiteSpace(previousUri) && previousUri.Equals(uri))
                    {
                        return;
                    }

                    using (var request = new HttpRequestMessage())
                    {
                        var sourceUri = new Uri(uri);

                        //If the source URI has a file URL scheme, do not form the RequestUri.
                        if (!sourceUri.IsFile)
                        {
                            request.RequestUri = sourceUri;
                        }
                      
                        var method = source.Value<string>("method");
                        var headers = (string)source.GetValue("headers", StringComparison.Ordinal);
                        var body = source.Value<Byte[]>("body");

                        view.Navigate(sourceUri, view.Name, body, headers);
                        return;
                    }
                }
            }

            view.Navigate(new Uri(BLANK_URL));
        }

        /// <summary>
        /// Receive events/commands directly from JavaScript through the
        /// <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public override void ReceiveCommand(WebBrowser view, int commandId, JArray args)
        {
            switch (commandId)
            {
                case CommandGoBack:
                    if (view.CanGoBack) view.GoBack();
                    break;
                case CommandGoForward:
                    if (view.CanGoForward) view.GoForward();
                    break;
                case CommandReload:
                    view.Refresh();
                    break;
                default:
                    throw new InvalidOperationException(
                        Invariant($"Unsupported command '{commandId}' received by '{typeof(ReactWebViewManager)}'."));
            }
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup by the <see cref="ReactWebViewManager"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, WebBrowser view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.LoadCompleted -= OnLoadCompleted;
            view.Navigating -= OnNavigationStarting;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="WebBrowser"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override WebBrowser CreateViewInstance(ThemedReactContext reactContext)
        {
            return new WebBrowser();
        }

        /// <summary>
        /// Subclasses can override this method to install custom event
        /// emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, WebBrowser view)
        {
            base.AddEventEmitters(reactContext, view);
            view.LoadCompleted += OnLoadCompleted;
            view.Navigating += OnNavigationStarting;
        }

        private void OnLoadCompleted(object sender, NavigationEventArgs e)
        {
            var webView = (WebBrowser)sender;
            LoadFinished(webView, e.Uri?.OriginalString);

            if (webView.IsLoaded)
            {
                var script = default(string);

                if (_injectedJS.TryGetValue(webView.GetTag(), out script) && !string.IsNullOrWhiteSpace(script))
                {
                    string[] args = { script };
                    try
                    {
                        webView.InvokeScript("eval", args);
                    }
                    catch (Exception ex)
                    {
                        LoadFailed(webView, "Loaded", ex.Message);
                    }
                }
            }
            else
            {
                LoadFailed(webView, "Unknown Error loading webview.", null);
            }
        }

        private static void OnNavigationStarting(object sender, NavigatingCancelEventArgs e)
        {
            var webView = (WebBrowser)sender;

            webView.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new WebViewLoadingEvent(
                         webView.GetTag(),
                         "Start",
                         e.Uri?.OriginalString,
                         true,
                         "Title Unavailable",
                         webView.CanGoBack,
                         webView.CanGoForward));
        }

        private static void LoadFinished(WebBrowser webView, string uri)
        {
            webView.GetReactContext().GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                         new WebViewLoadingEvent(
                            webView.GetTag(),
                            "Finish",
                            uri,
                            false,
                            "Title Unavailable",
                            webView.CanGoBack,
                            webView.CanGoForward));
        }

        private void LoadFailed(WebBrowser webView, string status, string message)
        {
            var reactContext = webView.GetReactContext();
            reactContext.GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new WebViewLoadingErrorEvent(
                        webView.GetTag(),
                        status,
                        message));
        }
    }
}
