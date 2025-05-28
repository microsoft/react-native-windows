/**
 * @format
 * @flow
 */

const nativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');

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

/*
const CalendarView = nativeComponentRegistry.get('CalendarView', () => ({
  uiViewClassName: 'FX_CalendarView',
  directEventTypes: {},
  validAttributes: {width: true, height: true},
}));
*/

export {StackPanel, Button};
