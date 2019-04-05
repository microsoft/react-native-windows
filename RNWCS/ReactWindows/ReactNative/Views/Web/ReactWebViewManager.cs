// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.Web.Events;
using ReactNativeWebViewBridge;
using System;
using Windows.UI.Xaml.Controls;
using Windows.Web.Http;
using static System.FormattableString;

namespace ReactNative.Views.Web
{
    /// <summary>
    /// A view manager responsible for rendering webview.
    /// </summary>
    public class ReactWebViewManager : SimpleViewManager<WebView>
    {
        private const string BLANK_URL = "about:blank";

        private const int CommandGoBack = 1;
        private const int CommandGoForward = 2;
        private const int CommandReload = 3;
        private const int CommandStopLoading = 4;
        private const int CommandPostMessage = 5;
        private const int CommandInjectJavaScript = 6;

        private const string BridgeName = "__REACT_WEB_VIEW_BRIDGE";

        private readonly ViewKeyedDictionary<WebView, WebViewData> _webViewData = new ViewKeyedDictionary<WebView, WebViewData>();
        private readonly ReactContext _context;

        /// <summary>
        /// Instantiates the <see cref="ReactWebViewManager"/>.
        /// </summary>
        /// <param name="context">The React context.</param>
        public ReactWebViewManager(ReactContext context)
        {
            _context = context;
        }

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
                    { "stopLoading", CommandStopLoading },
                    { "postMessage", CommandPostMessage },
                    { "injectJavaScript", CommandInjectJavaScript },
                };
            }
        }

        /// <summary>
        /// Sets the background color for the <see cref="WebView"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.BackgroundColor, CustomType = "Color")]
        public void SetBackgroundColor(WebView view, uint? color)
        {
            if (color.HasValue)
            {
                view.DefaultBackgroundColor = ColorHelpers.Parse(color.Value);
            }
            else
            {
                view.ClearValue(WebView.DefaultBackgroundColorProperty);
            }
        }

        /// <summary>
        /// Sets whether JavaScript is enabled or not.
        /// </summary>
        /// <param name="view">A webview instance.</param>
        /// <param name="enabled">A flag signaling whether JavaScript is enabled.</param>
        [ReactProp("javaScriptEnabled")]
        public void SetJavaScriptEnabled(WebView view, bool enabled)
        {
            view.Settings.IsJavaScriptEnabled = enabled;
        }

        /// <summary>
        /// Sets whether Indexed DB is enabled or not.
        /// </summary>
        /// <param name="view">A webview instance.</param>
        /// <param name="enabled">A flag signaling whether Indexed DB is enabled.</param>
        [ReactProp("indexedDbEnabled")]
        public void SetIndexedDbEnabled(WebView view, bool enabled)
        {
            view.Settings.IsIndexedDBEnabled = enabled;
        }

        /// <summary>
        /// Sets the JavaScript to be injected when the page loads.
        /// </summary>
        /// <param name="view">A view instance.</param>
        /// <param name="injectedJavaScript">The JavaScript to inject.</param>
        [ReactProp("injectedJavaScript")]
        public void SetInjectedJavaScript(WebView view, string injectedJavaScript)
        {
            var webViewData = GetWebViewData(view);
            webViewData.InjectedJavaScript = injectedJavaScript;
        }

        /// <summary>
        /// Toggles whether messaging is enabled for the <see cref="WebView"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="messagingEnabled">
        /// <code>true</code> if messaging is allowed, otherwise <code>false</code>.
        /// </param>
        [ReactProp("messagingEnabled")]
        public void SetMessagingEnabled(WebView view, bool messagingEnabled)
        {
            var webViewData = GetWebViewData(view);
            if (messagingEnabled)
            {
                var bridge = new WebViewBridge(view.GetTag());
                bridge.MessagePosted += OnMessagePosted;
                webViewData.Bridge = bridge;
            }
            else if (webViewData.Bridge != null)
            {
                webViewData.Bridge.MessagePosted -= OnMessagePosted;
                webViewData.Bridge = null;
            }
        }

        /// <summary>
        /// Sets webview source.
        /// </summary>
        /// <param name="view">A webview instance.</param>
        /// <param name="source">A source for the webview (either static html or an uri).</param>
        [ReactProp("source")]
        public void SetSource(WebView view, JObject source)
        {
            var webViewData = GetWebViewData(view);
            webViewData.Source = source;
            webViewData.SourceUpdated = true;
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
        public override void ReceiveCommand(WebView view, int commandId, JArray args)
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
                case CommandStopLoading:
                    view.Stop();
                    break;
                case CommandPostMessage:
                    PostMessage(view, args[0].Value<string>());
                    break;
                case CommandInjectJavaScript:
                    InvokeScript(view, args[0].Value<string>());
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
        public override void OnDropViewInstance(ThemedReactContext reactContext, WebView view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.NavigationStarting -= OnNavigationStarting;
            view.DOMContentLoaded -= OnDOMContentLoaded;
            view.NavigationFailed -= OnNavigationFailed;
            view.NavigationCompleted -= OnNavigationCompleted;

            _webViewData.Remove(view);
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="WebView"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override WebView CreateViewInstance(ThemedReactContext reactContext)
        {
            var view = new WebView(WebViewExecutionMode.SeparateThread);
            var data = new WebViewData();
            _webViewData.AddOrUpdate(view, data);
            return view;
        }

        /// <summary>
        /// Subclasses can override this method to install custom event 
        /// emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, WebView view)
        {
            base.AddEventEmitters(reactContext, view);
            view.NavigationStarting += OnNavigationStarting;
            view.DOMContentLoaded += OnDOMContentLoaded;
            view.NavigationFailed += OnNavigationFailed;
            view.NavigationCompleted += OnNavigationCompleted;
        }

        /// <summary>
        /// Callback that will be triggered after all props are updated         
        /// </summary>
        /// <param name="view">The view instance.</param>
        protected override void OnAfterUpdateTransaction(WebView view)
        {
            var webViewData = GetWebViewData(view);
            if (webViewData.SourceUpdated)
            {
                NavigateToSource(view);
                webViewData.SourceUpdated = false;
            }
        }

        private void NavigateToSource(WebView view)
        {
            var webViewData = GetWebViewData(view);
            var source = webViewData.Source;
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
                    // HTML files need to be loaded with the ms-appx-web schema.
                    uri = uri.Replace("ms-appx:", "ms-appx-web:");

                    string previousUri = view.Source?.OriginalString;
                    if (!String.IsNullOrWhiteSpace(previousUri) && previousUri.Equals(uri))
                    {
                        return;
                    }

                    using (var request = new HttpRequestMessage())
                    {
                        request.RequestUri = new Uri(uri);

                        var method = source.Value<string>("method");
                        if (method != null)
                        {
                            if (method.Equals("GET"))
                            {
                                request.Method = HttpMethod.Get;
                            }
                            else if (method.Equals("POST"))
                            {
                                request.Method = HttpMethod.Post;
                            }
                            else
                            {
                                throw new InvalidOperationException(
                                    Invariant($"Unsupported method '{method}' received by '{typeof(ReactWebViewManager)}'."));
                            }
                        }
                        else
                        {
                            request.Method = HttpMethod.Get;
                        }

                        var headers = (JObject)source.GetValue("headers", StringComparison.Ordinal);
                        if (headers != null)
                        {
                            foreach (var header in headers)
                            {
                                request.Headers.Append(header.Key, header.Value.Value<string>());
                            }
                        }

                        var body = source.Value<string>("body");
                        if (body != null)
                        {
                            request.Content = new HttpStringContent(body);
                        }

                        view.NavigateWithHttpRequestMessage(request);
                        return;
                    }
                }
            }

            view.Navigate(new Uri(BLANK_URL));
        }

        private async void InvokeScript(WebView view, string script)
        {
            await view.InvokeScriptAsync("eval", new[] { script }).AsTask().ConfigureAwait(false);
        }

        private void PostMessage(WebView view, string message)
        {
            var json = new JObject
            {
                { "data", message },
            };

            var script = "(function() {" +
                  "var event;" +
                  $"var data = {json.ToString(Formatting.None)};" +
                  "try {" +
                    "event = new MessageEvent('message', data);" +
                  "} catch (e) {" +
                    "event = document.createEvent('MessageEvent');" +
                    "event.initMessageEvent('message', true, true, data.data, data.origin, data.lastEventId, data.source);" +
                  "}" +
                  "document.dispatchEvent(event);" +
                "})();";

            InvokeScript(view, script);
        }

        private void OnNavigationStarting(object sender, WebViewNavigationStartingEventArgs e)
        {
            var webView = (WebView)sender;
            var tag = webView.GetTag();
            webView.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new WebViewLoadEvent(
                         tag,
                         WebViewLoadEvent.TopLoadingStart,
                         e.Uri?.OriginalString,
                         true,
                         webView.DocumentTitle,
                         webView.CanGoBack,
                         webView.CanGoForward));

            var bridge = GetWebViewData(webView).Bridge;
            if (bridge != null)
            {
                webView.AddWebAllowedObject(BridgeName, bridge);
            }
        }

        private void OnDOMContentLoaded(WebView sender, WebViewDOMContentLoadedEventArgs args)
        {
            var webViewData = GetWebViewData(sender);
            if (webViewData.Bridge != null)
            {
                InvokeScript(sender, $"window.postMessage = data => {BridgeName}.postMessage(String(data))");
            }
        }

        private void OnNavigationFailed(object sender, WebViewNavigationFailedEventArgs e)
        {
            var webView = (WebView)sender;
            webView.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new WebViewLoadingErrorEvent(
                        webView.GetTag(),
                        e.WebErrorStatus));
        }

        private void OnNavigationCompleted(object sender, WebViewNavigationCompletedEventArgs e)
        {
            var webView = (WebView)sender;
            webView.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new WebViewLoadEvent(
                        webView.GetTag(),
                        WebViewLoadEvent.TopLoadingFinish,
                        e.Uri?.OriginalString,
                        false,
                        webView.DocumentTitle,
                        webView.CanGoBack,
                        webView.CanGoForward));

            if (e.IsSuccess)
            {
                var injectedJavaScript = GetWebViewData(webView).InjectedJavaScript;
                if (injectedJavaScript != null)
                {
                    InvokeScript(webView, injectedJavaScript);
                }
            }
        }

        private void OnMessagePosted(object sender, MessagePostedEventArgs e)
        {
            _context.GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new MessageEvent(e.Tag, e.Message));
        }

        private WebViewData GetWebViewData(WebView webView)
        {
            return _webViewData[webView];
        }

        class WebViewData
        {
            public WebViewBridge Bridge { get; set; }

            public JObject Source { get; set; }

            public bool SourceUpdated { get; set; }

            public string InjectedJavaScript { get; set; }
        }
    }
}
