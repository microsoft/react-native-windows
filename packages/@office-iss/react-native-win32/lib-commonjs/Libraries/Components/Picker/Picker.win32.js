"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const RCTPickerItem = react_native_1.requireNativeComponent('RCTPickerItem');
const RCTPicker = react_native_1.requireNativeComponent('RCTPicker');
class PickerItem extends react_1.default.Component {
    render() {
        return react_1.default.createElement(RCTPickerItem, Object.assign({}, this.props));
    }
}
;
function getValueChangeEvent(pickerProps, children) {
    return (event) => {
        const { onValueChange } = pickerProps;
        if (!onValueChange || !children) {
            return;
        }
        const pickerItems = children;
        const selectedIndex = event.nativeEvent.pickerItemIndex;
        if (selectedIndex >= 0 && selectedIndex < pickerItems.length) {
            onValueChange(pickerItems[selectedIndex].props.value, selectedIndex);
        }
        else {
            onValueChange(null, selectedIndex);
        }
    };
}
class Picker extends react_1.default.Component {
    render() {
        const { selectedValue, children } = this.props;
        let selectedIndex = -1;
        if (selectedValue && children) {
            selectedIndex = children.findIndex(pickerItem => {
                return pickerItem.props.value === selectedValue;
            });
        }
        const onValueChange = getValueChangeEvent(this.props, children);
        const nativeProps = Object.assign(Object.assign({}, this.props), { selectedIndex,
            onValueChange });
        return react_1.default.createElement(RCTPicker, Object.assign({}, nativeProps), children);
    }
}
Picker.Item = PickerItem;
module.exports = Picker;
//# sourceMappingURL=Picker.win32.js.map