/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @providesModule PlatformViewPropTypes
 * @flow
 */

const PropTypes = require('prop-types');

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
};
