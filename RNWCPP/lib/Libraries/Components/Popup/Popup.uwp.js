// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
Object.defineProperty(exports, "__esModule", { value: true });
var React = require("react");
var react_native_1 = require("react-native");
var styles = react_native_1.StyleSheet.create({
    rctPopup: {
        position: 'absolute'
    },
});
var RCTPopup = react_native_1.requireNativeComponent('RCTPopup');
/**
 * Renders a popup component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword popup
 */
var Popup = /** @class */ (function (_super) {
    __extends(Popup, _super);
    function Popup(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { target: undefined, targetRef: null };
        return _this;
    }
    Popup.getDerivedStateFromProps = function (nextProps, prevState) {
        // Check if we're given a new target property; we need to resolve it to a node handle before render
        if (prevState.targetRef !== nextProps.target) {
            // Map the 'target' property to a node tag to use in the native layer
            /* tslint:disable-next-line no-any */
            var newTarget = react_native_1.findNodeHandle(nextProps.target);
            return {
                target: newTarget,
                targetRef: nextProps.target
            };
        }
        return prevState;
    };
    Popup.prototype.render = function () {
        var props = __assign({}, this.props);
        props.style = [styles.rctPopup, this.props.style];
        return (React.createElement(RCTPopup, __assign({}, props, { target: this.state.target })));
    };
    return Popup;
}(React.Component));
exports.Popup = Popup;
exports.default = Popup;
//# sourceMappingURL=Popup.uwp.js.map