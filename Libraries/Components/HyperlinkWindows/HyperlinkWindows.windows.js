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

var PropTypes = require('prop-types');
var React = require('React');
var ReactNativeViewAttributes = require('ReactNativeViewAttributes');
var ViewPropTypes = require('ViewPropTypes');
var requireNativeComponent = require('requireNativeComponent');

const ColorPropType = require('ColorPropType');
const EdgeInsetsPropType = require('EdgeInsetsPropType');
const StyleSheetPropType = require('StyleSheetPropType');
const Text = require('Text');
const TextStylePropTypes = require('TextStylePropTypes');
const TouchableWithoutFeedback = require('TouchableWithoutFeedback');

const stylePropType = StyleSheetPropType(TextStylePropTypes);

class HyperlinkWindows extends React.Component {
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
  };

  static propTypes = {
    ...Text.PropTypes,
  };

  static defaultProps = {
  };

  render() {
    return (
        <TouchableWithoutFeedback 
            onLayout={this.props.onLayout}
            onPress={this._onPress}
            onLongPress={this._onLongPress}
            rejectResponderTermination={true}
            nativeID={this.props.nativeID}
            testID={this.props.testID}
        >
            <RCTHyperlink 
                { ...this.props } 
            />
        </TouchableWithoutFeedback>
    );
  }

  _onPress = (e) => {
    this.props.onPress && this.props.onPress(e);
  }

  _onLongPress = (e) => {
      this.props.onLongPress && this.props.onLongPress(e);
  }
}

var RCTHyperlink = requireNativeComponent(
    'RCTHyperlink',
    HyperlinkWindows
);

module.exports = HyperlinkWindows;