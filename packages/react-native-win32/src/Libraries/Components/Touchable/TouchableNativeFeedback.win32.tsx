/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule TouchableNativeFeedback
 */

'use strict';

import * as React from 'react';
import { requireNativeComponent } from 'react-native';

import { ITouchableNativeFeedbackProps } from './TouchableNativeFeedback.Props';

class TouchableNativeFeedback extends React.Component<ITouchableNativeFeedbackProps, {}> {
  public render() {
    return <RCTTouchableNativeFeedback {...this.props}>{this.props.children}</RCTTouchableNativeFeedback>;
  }
}

const RCTTouchableNativeFeedback = requireNativeComponent('RCTTouchableNativeFeedback');

export = TouchableNativeFeedback;
