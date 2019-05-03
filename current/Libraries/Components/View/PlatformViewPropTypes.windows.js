/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @providesModule PlatformViewPropTypes
 * @flow
 */

const PropTypes = require('prop-types');
const { DeprecatedAccessibilityTraits } = require('DeprecatedViewAccessibility');

const WindowsAccessibilityTraits = Array.from(DeprecatedAccessibilityTraits).concat(['listItem']);

module.exports = {
  /**
   * Controls how view can be manipulated using gestures. We have not yet added
   * support for inertia or rails manipulation modes.
   *
   * Possible values:
   *
   *  - `'system'` - (default) The view responds in the way designated by the
   *    native system
   *  - `'none'` - The view does not respond to any touch.
   *  - `'translateX'` - The view responds to X-dimension gestures.
   *  - `'translateY'` - The view responds to Y-dimension gestures.
   *  - `'rotate'` - The view responds to rotate gestures.
   *  - `'scale'` - The view responds to scale gestures.
   *  - `'all'` - The view responds to all gesture types.
   *
   * @platform windows
   */
  manipulationModes: PropTypes.arrayOf(
    PropTypes.oneOf([
      'system',
      'none',
      'translateX',
      'translateY',
      'rotate',
      'scale',
      'all',
    ])),

  /**
   * A short string description that will appear in a tooltip if the user
   * hovers their pointer over the View for a short duration.
   *
   * @platform windows
   */
  tooltip: PropTypes.string,

  /**
   * Controls how view is important for accessibility which is if it
   * fires accessibility events and if it is reported to accessibility services
   * that query the screen. Works for Android only.
   *
   * @platform windows
   *
   * See http://facebook.github.io/react-native/docs/view.html#importantforaccessibility
   */
  importantForAccessibility: PropTypes.oneOf([
    'auto',
    'yes',
    'no',
    'no-hide-descendants',
    'yes-dont-hide-descendants',
  ]),

  /**
   * Provides additional traits to screen reader. By default no traits are
   * provided unless specified otherwise in element.
   *
   * You can provide one trait or an array of many traits.
   *
   * @platform windows
   *
   * See http://facebook.github.io/react-native/docs/view.html#accessibilitytraits
   */
  accessibilityTraits: PropTypes.oneOfType([
    PropTypes.oneOf(WindowsAccessibilityTraits),
    PropTypes.arrayOf(PropTypes.oneOf(WindowsAccessibilityTraits)),
  ]),

  /**
   * Mouse move handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */
  onMouseMove: PropTypes.func,

  /**
   * Mouse move (capturing phase) handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */
  onMouseMoveCapture: PropTypes.func,  

  /**
   * Mouse over handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */
  onMouseOver: PropTypes.func,

  /**
   * Mouse over (capturing phase) handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */
  onMouseOverCapture: PropTypes.func,  

  /**
   * Mouse out handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */
  onMouseOut: PropTypes.func,

  /**
   * Mouse out (capturing phase) handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */
  onMouseOutCapture: PropTypes.func,  

  /**
   * Mouse enter handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */

  onMouseEnter: PropTypes.func,
  /**
   * Mouse leave handler. 
   * This is explicitly defined here in order to be able to have an associated "handler detection" property 
   * on the native side
   *
   * @platform windows
   */
  onMouseLeave: PropTypes.func,
};
