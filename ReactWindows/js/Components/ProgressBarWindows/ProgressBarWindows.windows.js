/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @providesModule ProgressBarWindows
 * @flow
 */
'use strict';

var React = require('React');
var ReactPropTypes = require('ReactPropTypes');
var ReactNativeViewAttributes = require('ReactNativeViewAttributes');
var View = require('View');
var ColorPropType = require('ColorPropType');

var requireNativeComponent = require('requireNativeComponent');



var ProgressBarWindows = React.createClass({
  
  propTypes: {
    ...View.propTypes,
    
    /**
     * If the progress bar will show indeterminate progress. 
     * true: rolling dots
     * false: filling rectangle that animates progress change.
     */
    indeterminate: ReactPropTypes.bool,
    /**
     * The progress value (between 0 and 1).
     */
    progress: ReactPropTypes.number,
    /**
     * Color of the progress bar.
     */
    color: ColorPropType,
    
  },
  
  getDefaultProps: function() {
    return {
      indeterminate: true
    };
  },
  
  
  render: function() {
    return <NativeWindowsProgressBar {...this.props}/> ;
  },
});

var NativeWindowsProgressBar = requireNativeComponent('WindowsProgressBar', ProgressBarWindows);

module.exports = ProgressBarWindows;
