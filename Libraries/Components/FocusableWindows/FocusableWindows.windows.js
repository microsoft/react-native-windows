/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 * All rights reserved.
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * Helper component for building keyboard focusable custom controls when the original native one has no such support
 *
 * For example, assuming a SomeComponent already exists, a custom KeyboardSupportingComponent can
 * be built this way (at the minimum):
 *
 *   class KeyboardSupportingComponent extends React.Component {
 *     render () {
 *
 *         styleSet = FocusableWindows.splitStyle(this.prop.styles);
 *
 *         return (
 *            <FocusableWindows
 *               ref = {(f) => this._focusable = v}
 *               style = {styleSet.focusableStyle}
 *               onKeyDown = {...}
 *               handledKeyDownKeys: {[FocusableWindows.keys.Enter]}
 *            >
 *              <SomeComponent
 *                  {...this.props}
 *                  ref = {(c) => this._child = c}
 *                  style = {styleSet.childStyle}
 *              />
 *            </FocusableWindows>
 *         );
 *      }
 *
 *      // important for animations
 *      setNativeProps (props) {
 *          var splitProps = FocusableWindows.splitNativeProps(props);
 *          if (splitProps.focusableProps !== undefined) {
 *             this._focusable.setNativeProps(splitProps.focusableProps)
 *          }
 *          if (splitProps.childProps !== undefined) {
 *             this._child.setNativeProps(splitProps.childProps);
 *          }
 *      }
 *   }
 *
 * @providesModule FocusableWindows
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ReactNative = require('ReactNative');
var ViewPropTypes = require('ViewPropTypes');
var requireNativeComponent = require('requireNativeComponent');
var UIManager = require('UIManager');
const flattenStyle = require('flattenStyle');

class FocusableWindows extends React.Component {

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
    disableSystemFocusVisuals: PropTypes.bool,

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

  /**
   * Splits styles into two sets: one applying to focusable view, and the other one to the embedded child
   * The only style we care about is "transform"
   */
  static splitStyle(styles) {
      var sets = {};

      sets.focusableStyle = {};
      sets.childStyle = {};

      if (styles) {
        var flattenedStyles = flattenStyle(styles);

        for (var styleName in flattenedStyles) {
          if (styleName === 'transform') {
            sets.focusableStyle[styleName] = flattenedStyles[styleName];
          } else {
            sets.childStyle[styleName] = flattenedStyles[styleName];
          }
        }
      }

      return sets;
  }

  /**
   * Splits native properties used in setNativeProps into two sets: one applying to focusable view, and the other one to
   * the embedded child
   * The only style we care about is "transform"
   */
  static splitNativeProps(nativeProps) {
    var sets = {};

    sets.focusableProps = undefined;
    sets.childProps = nativeProps;

    if (nativeProps && nativeProps.style && 'transform' in nativeProps.style) {

      sets.focusableProps = {};
      sets.focusableProps.style = {};
      var childStyle = {};

      var atLeastOneChildProp = false;
      // There is no need to flatten the styles
      for (var styleName in nativeProps.style) {
        if (styleName === 'transform') {
          sets.focusableProps.style[styleName] = nativeProps.style[styleName];
        } else {
          childStyle[styleName] = nativeProps.style[styleName];
          atLeastOneChildProp = true;
        }
      }

      sets.childProps.style = childStyle;

      if (!atLeastOneChildProp) {
        // Check presence of any other prop
        for (var propName in sets.childProps) {
          if (propName !== 'style') {
            atLeastOneChildProp = true;
            break;
          }
        }
      }

      if (!atLeastOneChildProp) {
        sets.childProps = undefined;
      }
    }

    return sets;
}

  render() {
    return (
      <WindowsControl
        {...this.props}
        onFocus = {this._onFocus}
        onBlur = {this._onBlur}
        ref = {(ref) => this._setControlRef(ref)}
      />);
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

  setNativeProps(nativeProps) {
    if (this._controlRef) {
        this._controlRef.setNativeProps(nativeProps);
    }
  }
}

var WindowsControl = requireNativeComponent(
  'WindowsControl',
  FocusableWindows
);

module.exports = FocusableWindows;
