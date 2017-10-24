/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule ViewStylePropTypes
 * @flow
 */
'use strict';

var ColorPropType = require('ColorPropType');
var LayoutPropTypes = require('LayoutPropTypes');
const { PropTypes } = require('prop-types');
var ShadowPropTypesIOS = require('ShadowPropTypesIOS');
var TransformPropTypes = require('TransformPropTypes');

/**
 * Warning: Some of these properties may not be supported in all releases.
 */
var ViewStylePropTypes = {
  ...LayoutPropTypes,
  ...ShadowPropTypesIOS,
  ...TransformPropTypes,
  acrylicOpacity: PropTypes.number,
  acrylicSource: PropTypes.oneOf(['host', 'app']),
  acrylicTintColor: ColorPropType,
  backfaceVisibility: PropTypes.oneOf(['visible', 'hidden']),
  backgroundColor: ColorPropType,
  borderColor: ColorPropType,
  borderTopColor: ColorPropType,
  borderRightColor: ColorPropType,
  borderBottomColor: ColorPropType,
  borderLeftColor: ColorPropType,
  borderRadius: PropTypes.number,
  borderTopLeftRadius: PropTypes.number,
  borderTopRightRadius: PropTypes.number,
  borderBottomLeftRadius: PropTypes.number,
  borderBottomRightRadius: PropTypes.number,
  borderStyle: PropTypes.oneOf(['solid', 'dotted', 'dashed']),
  borderWidth: PropTypes.number,
  borderTopWidth: PropTypes.number,
  borderRightWidth: PropTypes.number,
  borderBottomWidth: PropTypes.number,
  borderLeftWidth: PropTypes.number,
  opacity: PropTypes.number,
  /**
   * (Android-only) Sets the elevation of a view, using Android's underlying
   * [elevation API](https://developer.android.com/training/material/shadows-clipping.html#Elevation).
   * This adds a drop shadow to the item and affects z-order for overlapping views.
   * Only supported on Android 5.0+, has no effect on earlier versions.
   * @platform android
   */
  elevation: PropTypes.number,
};

module.exports = ViewStylePropTypes;
