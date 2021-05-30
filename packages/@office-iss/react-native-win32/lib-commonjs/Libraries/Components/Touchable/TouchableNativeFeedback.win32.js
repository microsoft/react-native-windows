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
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
class TouchableNativeFeedback extends react_1.default.Component {
    render() {
        return react_1.default.createElement(RCTTouchableNativeFeedback, Object.assign({}, this.props), this.props.children);
    }
}
const RCTTouchableNativeFeedback = react_native_1.requireNativeComponent('RCTTouchableNativeFeedback');
module.exports = TouchableNativeFeedback;
//# sourceMappingURL=TouchableNativeFeedback.win32.js.map