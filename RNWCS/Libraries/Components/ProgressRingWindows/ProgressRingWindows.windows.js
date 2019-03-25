/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @providesModule ProgressRingWindows
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ReactNativeViewAttributes = require('ReactNativeViewAttributes');
var ViewPropTypes = require('ViewPropTypes');
var requireNativeComponent = require('requireNativeComponent');

var ColorPropType = require('ColorPropType');

class ProgressRingWindows extends React.Component {
  props: {color?: $FlowFixMe};

  static propTypes = {
    ...ViewPropTypes,
    /**
     * Color of the progress bar.
     */
    color: ColorPropType,
  };

  static defaultProps = {
    animating: true
  };

  render() {
    return <WindowsProgressRing {...this.props}/>;
  }
}

var WindowsProgressRing = requireNativeComponent(
    'WindowsProgressRing',
    ProgressRingWindows,
    {nativeOnly: {animating: true}}
);

module.exports = ProgressRingWindows;
