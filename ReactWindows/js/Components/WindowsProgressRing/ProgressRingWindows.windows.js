/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @providesModule ProgressRingWindows
 * @flow
 */
'use strict';

var React = require('React');
var ReactPropTypes = require('ReactPropTypes');
var ReactNativeViewAttributes = require('ReactNativeViewAttributes');
var View = require('View');
var requireNativeComponent = require('requireNativeComponent');
var ColorPropType = require('ColorPropType');

var ProgressRingWindows = React.createClass({  
  propTypes: {
    isActive: ReactPropTypes.bool,
    color: ColorPropType,
    onLayout: ReactPropTypes.func,
  },
  
  getDefaultProps: function() {
    return {
      isActive: true
    };
  },
  
  render: function() {
    return <NativeProgressRingView {...this.props}/> ;
  },
});

var NativeProgressRingView = requireNativeComponent('WindowsProgressRing', ProgressRingWindows);

module.exports = ProgressRingWindows;