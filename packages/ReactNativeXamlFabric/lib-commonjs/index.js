"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.Button = exports.StackPanel = exports.XamlHost = void 0;
const XamlHostNativeComponent_1 = __importDefault(require("./XamlHostNativeComponent"));
exports.XamlHost = XamlHostNativeComponent_1.default;
const nativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
//const ConditionallyIgnoredEventHandlers = require('react-native/Libraries/NativeComponent/ViewConfigIgnore').ConditionallyIgnoredEventHandlers;
const StackPanel = nativeComponentRegistry.get('StackPanel', () => ({
    uiViewClassName: 'FX_StackPanel',
    directEventTypes: {},
    validAttributes: { label: true, content: true },
}));
exports.StackPanel = StackPanel;
const Button = nativeComponentRegistry.get('Button', () => ({
    uiViewClassName: 'FX_Button',
    directEventTypes: {},
    validAttributes: { label: true, content: true, text: true, davis: true },
}));
exports.Button = Button;
//# sourceMappingURL=index.js.map