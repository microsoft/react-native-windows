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
   * Enables the view as a drop target. If this is set, the user will be able
   * to drag and drop files to the view and it will emit the four events:
   *
   *    `onDragEnter`
   *    `onDragOver`
   *    `onDrop` - contains the list of dropped files (their URIs to be precise)
   *    `onDragLeave`
   *
   * The native event in these callbacks contains the `dataTransfer` field that
   * implements the DataTransfer JS interface. Note, that unlike in the web, here
   * the event is readonly and thus changing `effectAllowed` and `dropEffect` fields
   * will do nothing.
   *
   * @platform windows
   */
  allowDrop: PropTypes.bool,

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
