import XamlHost from './XamlHostNativeComponent';

const nativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
//const ConditionallyIgnoredEventHandlers = require('react-native/Libraries/NativeComponent/ViewConfigIgnore').ConditionallyIgnoredEventHandlers;

const StackPanel = nativeComponentRegistry.get('StackPanel', () => ({
  uiViewClassName: 'FX_StackPanel',
  directEventTypes: {},
  validAttributes: {label: true, content: true},
}));

const Button = nativeComponentRegistry.get('Button', () => ({
  uiViewClassName: 'FX_Button',
  directEventTypes: {},
  validAttributes: {label: true, content: true, text: true, davis: true},
}));

export {XamlHost, StackPanel, Button};
