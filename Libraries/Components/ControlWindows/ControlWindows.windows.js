/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 * All rights reserved.
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @providesModule ControlWindows
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ViewPropTypes = require('ViewPropTypes');
var requireNativeComponent = require('requireNativeComponent');

class ControlWindows extends React.Component {

  static propTypes = {
    ...ViewPropTypes,
    
    /**
     * Controls whether the view is a tab stop. Useful for buttons and other
     * controls that can be focused.
     * 
     * @platform windows
     */
    isTabStop: PropTypes.bool,

    /**
     * Sets the tab index for the view.
     * 
     * @platform windows
     */
    tabIndex: PropTypes.number,

    /**
     * Sets the keyboard navigation mode for the view.
     * 
     * @platform windows
     */
    tabNavigation: PropTypes.oneOf([
      'local',
      'cycle',
      'once'
    ]),

    /**
     * Called when the view receives focus.
     * 
     * @platform windows
     */
    onFocus: PropTypes.func,

    /**
     * Called when the view focus is lost.
     * 
     * @platform windows
     */
    onBlur: PropTypes.func,

    /**
     * Set of keys that should be handled on key down by this component.
     * 
     * @platform windows
     */
    handledKeyDownKeys: PropTypes.arrayOf(PropTypes.number),

    /**
     * Set of keys that should be handled on key up by this component.
     * 
     * @platform windows
     */
    handledKeyUpKeys: PropTypes.arrayOf(PropTypes.number),

    /**
     * Called when key down while component has focus.
     * 
     * @platform windows
     */
    onKeyDown: PropTypes.func,

    /**
     * Called when key up while component has focus.
     * 
     * @platform windows
     */
    onKeyUp: PropTypes.func,
  };

  render() {
    return <WindowsControl {...this.props}/>;
  }
}

var WindowsControl = requireNativeComponent(
  'WindowsControl',
  ControlWindows
);

module.exports = ControlWindows;
