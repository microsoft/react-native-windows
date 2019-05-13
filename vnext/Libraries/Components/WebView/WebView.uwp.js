// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//
// Portions copyright for react-native-windows:
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

'use strict';

var React = require('React');
var ReactNative = require('ReactNative');
var StyleSheet = require('StyleSheet');
var UIManager = require('UIManager');
var View = require('View');

var keyMirror = require('fbjs/lib/keyMirror');
var requireNativeComponent = require('requireNativeComponent');
var resolveAssetSource = require('resolveAssetSource');

const createReactClass = require('create-react-class');

var RCT_WEBVIEW_REF = 'webview';

var WebViewState = keyMirror({
  IDLE: null,
  LOADING: null,
  ERROR: null,
});

/**
 * Renders a native WebView.
 */
var WebView = createReactClass({

  getInitialState: function() {
    return {
      viewState: WebViewState.IDLE,
      lastErrorEvent: null,
      startInLoadingState: true,
    };
  },

  getDefaultProps: function() {
    return {
      javaScriptEnabled : true,
    };
  },

  componentWillMount: function() {
    if (this.props.startInLoadingState) {
      this.setState({viewState: WebViewState.LOADING});
    }
  },

  render: function() {
    var otherView = null;

    if (this.state.viewState === WebViewState.LOADING) {
      otherView = this.props.renderLoading && this.props.renderLoading();
    } else if (this.state.viewState === WebViewState.ERROR) {
      var errorEvent = this.state.lastErrorEvent;
      otherView = this.props.renderError && this.props.renderError(
        errorEvent.domain,
        errorEvent.code,
        errorEvent.description);
    } else if (this.state.viewState !== WebViewState.IDLE) {
      console.error('RCTWebView invalid state encountered: ' + this.state.loading);
    }

    var webViewStyles = [styles.container, this.props.style];
    if (this.state.viewState === WebViewState.LOADING ||
      this.state.viewState === WebViewState.ERROR) {
      // if we're in either LOADING or ERROR states, don't show the webView
      webViewStyles.push(styles.hidden);
    }

    var source = this.props.source || {};
    if (this.props.html) {
      source.html = this.props.html;
    } else if (this.props.url) {
      source.uri = this.props.url;
    }

    var webView =
      <RCTWebView
        ref={RCT_WEBVIEW_REF}
        key="webViewKey"
        style={webViewStyles}
        source={resolveAssetSource(source)}
        injectedJavaScript={this.props.injectedJavaScript}
        javaScriptEnabled={this.props.javaScriptEnabled}
        indexedDbEnabled={this.props.indexedDbEnabled}
        contentInset={this.props.contentInset}
        automaticallyAdjustContentInsets={this.props.automaticallyAdjustContentInsets}
        onLoadingStart={this.onLoadingStart}
        onLoadingFinish={this.onLoadingFinish}
        onLoadingError={this.onLoadingError}
        testID={this.props.testID}
      />;

    return (
      <View style={styles.container}>
        {webView}
        {otherView}
      </View>
    );
  },

  goForward: function() {
    UIManager.dispatchViewManagerCommand(
      this.getWebViewHandle(),
      UIManager.RCTWebView.Commands.goForward,
      null
    );
  },

  goBack: function() {
    UIManager.dispatchViewManagerCommand(
      this.getWebViewHandle(),
      UIManager.RCTWebView.Commands.goBack,
      null
    );
  },

  reload: function() {
    UIManager.dispatchViewManagerCommand(
      this.getWebViewHandle(),
      UIManager.RCTWebView.Commands.reload,
      null
    );
  },

  /**
   * We return an event with a bunch of fields including:
   *  url, title, loading, canGoBack, canGoForward
   */
  updateNavigationState: function(event) {
    if (this.props.onNavigationStateChange) {
      this.props.onNavigationStateChange(event.nativeEvent);
    }
  },

  getWebViewHandle: function() {
    return ReactNative.findNodeHandle(this.refs[RCT_WEBVIEW_REF]);
  },

  onLoadingStart: function(event) {
    var onLoadStart = this.props.onLoadStart;
    onLoadStart && onLoadStart(event);
    this.updateNavigationState(event);
  },

  onLoadingError: function(event) {
    event.persist(); // persist this event because we need to store it
    var {onError, onLoadEnd} = this.props;
    onError && onError(event);
    onLoadEnd && onLoadEnd(event);
    console.error('Encountered an error loading page', event.nativeEvent);

    this.setState({
      lastErrorEvent: event.nativeEvent,
      viewState: WebViewState.ERROR
    });
  },

  onLoadingFinish: function(event) {
    var {onLoad, onLoadEnd} = this.props;
    onLoad && onLoad(event);
    onLoadEnd && onLoadEnd(event);
    this.setState({
      viewState: WebViewState.IDLE,
    });
    this.updateNavigationState(event);
  },
});

var RCTWebView = requireNativeComponent('RCTWebView', WebView);

var styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  hidden: {
    height: 0,
    flex: 0, // disable 'flex:1' when hiding a View
  },
  loadingView: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  loadingProgressBar: {
    height: 20,
  },
});

module.exports = WebView;
