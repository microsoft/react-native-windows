/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @providesModule ProgressRingWindows
 * @flow
 */
'use strict';

var React = require('React');
var ReactPropTypes = require('react/lib/ReactPropTypes');
var ReactNativeViewAttributes = require('ReactNativeViewAttributes');
var View = require('View');
var requireNativeComponent = require('requireNativeComponent');
var ColorPropType = require('ColorPropType');
const ViewPropTypes = require('ViewPropTypes')

var ProgressRingWindows = React.createClass({  
  propTypes: {
    ...ViewPropTypes,
    /**
     * Color of the progress bar.
     */
    color: ColorPropType,
  },
  
  getDefaultProps: function() {
    return {
      animating: true
    };
  },
  
  render: function() {
    return <WindowsProgressRing {...this.props}/> ;
  },
});

var WindowsProgressRing = requireNativeComponent(
    'WindowsProgressRing', 
    ProgressRingWindows,
    {nativeOnly: {animating: true}}
);

module.exports = ProgressRingWindows;
