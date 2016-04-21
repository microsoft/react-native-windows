/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @providesModule ProgressBarWindows
 * @flow
 */
'use strict';

var React = require('React');
var ReactElement = require('ReactElement');
var ReactPropTypes = require('ReactPropTypes');
var UIManager = require('UIManager');
var View = require('View');
var requireNativeComponent = require('requireNativeComponent');

type Event = Object;


var ProgressBarWindows = React.createClass({
  
  propTypes: {
    ...View.propTypes,
    
    /**
      * Summary:
      *     Initial value that indicates whether the progress bar reports generic
      *     progress with a repeating pattern or reports progress based on the Value property.
      */
    isIndeterminate: ReactPropTypes.bool,

    /**
     * Summary: 
     *      Initial setting of the range control, which may be coerced.
     */
     value: ReactPropTypes.number,
  },
  
  
  render: function() {
    return (
      <NativeWindowsProgressBar
        style={this.props.style}
        isIndeterminate={this.props.isIndeterminate}
        value={this.props.value}
      />
    );
  },
});

var NativeWindowsProgressBar = requireNativeComponent('WindowsProgressBar', ProgressBarWindows);

module.exports = ProgressBarWindows;
