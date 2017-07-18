/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @providesModule ProgressRingWindows
 * @flow
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ReactNativeViewAttributes = require('ReactNativeViewAttributes');
var View = require('View');
var requireNativeComponent = require('requireNativeComponent');

var ColorPropType = require('ColorPropType');

var ProgressRingWindows = React.createClass({  
  propTypes: {
    ...View.propTypes,
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
