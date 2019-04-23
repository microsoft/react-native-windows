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
Object.defineProperty(exports, "__esModule", { value: true });
var React = require("react");
var PickerItem = /** @class */ (function (_super) {
    __extends(PickerItem, _super);
    function PickerItem() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    PickerItem.prototype.render = function () {
        return null;
    };
    return PickerItem;
}(React.Component));
var Picker = /** @class */ (function (_super) {
    __extends(Picker, _super);
    function Picker() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    Picker.prototype.render = function () {
        return null;
    };
    Picker.Item = PickerItem;
    return Picker;
}(React.Component));
exports.Picker = Picker;
exports.default = Picker;
//# sourceMappingURL=PickerUWP.js.map