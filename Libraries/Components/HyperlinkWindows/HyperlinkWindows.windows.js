/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 * All rights reserved.
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @providesModule HyperlinkWindows
 */
'use strict';

const ColorPropType = require('ColorPropType');
const EdgeInsetsPropType = require('EdgeInsetsPropType');
const PropTypes = require('prop-types');
const React = require('React');
const ReactNative = require('ReactNative');
const requireNativeComponent = require('requireNativeComponent');
const ReactNativeViewAttributes = require('ReactNativeViewAttributes');
const StyleSheetPropType = require('StyleSheetPropType');
const Text = require('Text');
const TextStylePropTypes = require('TextStylePropTypes');
const TouchableWithoutFeedback = require('TouchableWithoutFeedback');
const UIManager = require('UIManager');
const ViewPropTypes = require('ViewPropTypes');

const stylePropType = StyleSheetPropType(TextStylePropTypes);

class HyperlinkWindows extends React.Component {
  displayName: string = 'Hyperlink';
  props: {
    ellipsizeMode: 'head' | 'middle' | 'tail' | 'clip',
    numberOfLines: number,
    textBreakStrategy: 'simple' | 'highQuality' | 'balanced',
    onLayout: Function,
    onPress: Function,
    onLongPress: Function,
    pressRetentionOffset: EdgeInsetsPropType,
    selectable: boolean,
    selectionColor: ColorPropType,
    suppressHighlighting: boolean,
    style: stylePropType,
    testID: string,
    nativeID: string,
    allowFontScaling: boolean,
    accessible: boolean,
    adjustsFontSizeToFit: boolean,
    minimumFontScale: number,
    disabled: boolean,
    onFocus: Function,
  };

  static propTypes = {
    ...Text.propTypes,
  };

  static defaultProps = {
  };

  _focusable: any;

  _longPressSent: boolean;

  render() {
    return (
        <TouchableWithoutFeedback 
            onLayout={this.props.onLayout}
            onLongPress={this._onLongPress}
            rejectResponderTermination={true}
            nativeID={this.props.nativeID}
            testID={this.props.testID}>
          <RCTHyperlink 
              { ...this.props }
              ref={ this._setFocusableRef }
              onFocus={ this._onFocus }
              onClick={ this._onClick }
          />
        </TouchableWithoutFeedback>
    );
  }
 
  _setFocusableRef = (ref) => {
    this._focusable = ref;
  }

  _onFocus = (e) => {
    this.props.onFocus && this.props.onFocus(e);
  }

  _onClick = (e) => {
    if (!this._longPressSent) {
      this.props.onPress && this.props.onPress(e);
    }

    this._longPressSent = false;
  }

  _onLongPress = (e) => {
    this._longPressSent = true;
    this.props.onLongPress && this.props.onLongPress(e);
  }

  focus() {
    if (this._focusable) {
      UIManager.dispatchViewManagerCommand(
        ReactNative.findNodeHandle(this._focusable),
        UIManager.WindowsControl.Commands.focus,
        null);
    }
  }

  blur() {
    if (this._focusable) {
      UIManager.dispatchViewManagerCommand(
        ReactNative.findNodeHandle(this._focusable),
        UIManager.WindowsControl.Commands.blur,
        null);
    }
  }
}

var RCTHyperlink = requireNativeComponent(
    'RCTHyperlink',
    HyperlinkWindows
  );

module.exports = HyperlinkWindows;