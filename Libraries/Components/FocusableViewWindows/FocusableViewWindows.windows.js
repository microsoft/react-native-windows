/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 * All rights reserved.
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @providesModule FocusableViewWindows
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ReactNative = require('ReactNative');
var ViewPropTypes = require('ViewPropTypes');
var requireNativeComponent = require('requireNativeComponent');
var UIManager = require('UIManager');
const flattenStyle = require('flattenStyle');

class FocusableViewWindows extends React.Component {

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
     * Controls whether control should use system default provided focus rects
     *
     * @platform windows
     */
    useSystemFocusVisuals: PropTypes.bool,

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

  static keys = UIManager.WindowsControl.Constants.Keys;

  render() {
    return <WindowsControl
              {...this.props}
              onFocus = {this._onFocus}
              onBlur = {this._onBlur}
              ref = {(ref) => this._setControlRef(ref)}
            />;
  }

  _setControlRef(ref) {
    this._controlRef = ref;
  }

  _onFocus = (e): void => {
    if (this.props.onFocus) {
      this.props.onFocus(e);
    }
  }

  _onBlur = (e): void => {
    if (this.props.onBlur) {
      this.props.onBlur(e);
    }
  }

  focus() {
    if (this._controlRef) {
      UIManager.dispatchViewManagerCommand(
        ReactNative.findNodeHandle(this._controlRef),
        UIManager.WindowsControl.Commands.focus,
        null);
    }
  }

  blur() {
    if (this._controlRef) {
      UIManager.dispatchViewManagerCommand(
        ReactNative.findNodeHandle(this._controlRef),
        UIManager.WindowsControl.Commands.blur,
        null);
    }
  }
}

var WindowsControl = requireNativeComponent(
  'WindowsControl',
  FocusableViewWindows
);

module.exports = FocusableViewWindows;
