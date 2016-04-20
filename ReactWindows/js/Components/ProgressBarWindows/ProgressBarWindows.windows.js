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
  
  render: function() {
    return (
      <NativeWindowsProgressBar/>
    );
  },
});

var NativeWindowsProgressBar = requireNativeComponent('WindowsProgressBar', ProgressBarWindows);

module.exports = ProgressBarWindows;
