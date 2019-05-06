/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @providesModule ProgressRingWindows
 */
'use strict';

var React = require('React');
var ViewPropTypes = require('ViewPropTypes');
var requireNativeComponent = require('requireNativeComponent');

var ColorPropType = require('DeprecatedColorPropType');

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
