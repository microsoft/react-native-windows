/**
 * @providesModule EnterString
 */
'use strict';
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const react_native_1 = require("react-native");
const react_1 = __importDefault(require("react"));
const PropTypes = require("prop-types");
class EnterStringNative extends react_1.default.Component {
    render() {
        return react_1.default.createElement(RCTEnterString, Object.assign({}, this.props));
    }
}
// mixins: [NativeMethodsMixin],
/**
 * `NativeMethodsMixin` will look for this when invoking `setNativeProps`. We
 * make `this` look like an actual native component class.
 */
/*
viewConfig: {
  uiViewClassName: 'RCTEnterString',
  validAttributes: {
      ...ReactNativeViewAttributes.RCTView,
      text: true,
      disabled: true,
      label: true,
  },
},
*/
// TODO figure out how to get a real type here
EnterStringNative.propTypes = {
    // React.ValidationMap<IEnterStringNativeProps> = {
    // ...View.propTypes,
    /**
     * Text to display inside the EnterString
     */
    value: PropTypes.string,
    /**
     * If true, disable all interactions for this component.
     */
    disabled: PropTypes.bool,
    /**
     * Label
     */
    label: PropTypes.string,
    /**
     * Callback when text is changed
     */
    onChanged: PropTypes.func,
};
EnterStringNative.DefaultProps = {
    disabled: false,
    value: '',
    label: '',
    onChanged: null,
};
const RCTEnterString = react_native_1.requireNativeComponent('RCTEnterString');
class EnterString extends react_1.default.Component {
    render() {
        const args = Object.assign({}, this.props);
        if (args.onChanged) {
            args.onChanged = this._onChangedNative.bind(this);
        } // TODO how should this work without the cast
        return react_1.default.createElement(EnterStringNative, Object.assign({}, args));
    }
    _onChangedNative(event) {
        if (this.props.onChanged) {
            this.props.onChanged(event.nativeEvent.text);
        }
    }
}
exports.default = EnterString;
EnterString.DefaultProps = {
    disabled: false,
    value: '',
    label: '',
    onChanged: null,
};
//# sourceMappingURL=EnterString.win32.js.map