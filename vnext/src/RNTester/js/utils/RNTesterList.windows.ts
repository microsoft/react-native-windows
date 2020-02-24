/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

'use strict';
import React = require('react');

interface IRNTesterExample {
  key: string;
  // tslint:disable-next-line no-reserved-keywords
  module: IRNTesterModule;
}

interface IRNTesterModule {
  title: string;
  description: string;
  external: boolean;
  examples: [IRNTesterModuleExample];
}

interface IRNTesterModuleExample {
  title: string;
  description: string;
  render(): React.Component;
}

const ComponentExamples: Array<IRNTesterExample> = [
  {
    key: 'ActivityIndicatorExample',
    module: require('react-native/RNTester/js/examples/ActivityIndicator/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('react-native/RNTester/js/examples/Button/ButtonExample'),
  },
  {
    key: 'CheckBoxExample',
    module: require('react-native/RNTester/js/examples/CheckBox/CheckBoxExample'),
  },
  {
    key: 'CustomViewExample',
    module: require('./../examples-win/CustomView/CustomViewExample'),
  },
  {
    key: 'DatePickerExample',
    module: require('./../examples-win/DatePicker/DatePickerExample'),
  },
  {
    key: 'FastTextExample',
    module: require('./../examples-win/FastText/FastTextExample'),
  },
  {
    key: 'FlatListExample',
    module: require('react-native/RNTester/js/examples/FlatList/FlatListExample'),
  },
  {
    key: 'FlyoutExample',
    module: require('./../examples-win/Flyout/FlyoutExample'),
  },
  {
    key: 'GlyphExample',
    module: require('./../examples-win/Glyph/GlyphExample'),
  },
  {
    key: 'ImageExample',
    module: require('react-native/RNTester/js/examples/Image/ImageExample'),
  },
  {
    key: 'MultiColumnExample',
    module: require('react-native/RNTester/js/examples/MultiColumn/MultiColumnExample'),
  },
  {
    key: 'NewAppScreenExample',
    module: require('react-native/RNTester/js/examples/NewAppScreen/NewAppScreenExample'),
  },
  {
    key: 'PickerExample',
    module: require('react-native/RNTester/js/examples/Picker/PickerExample'),
  },
  {
    key: 'PickerWindowsExample',
    module: require('./../examples-win/Picker/PickerWindowsExample'),
  },
  {
    key: 'PopupExample',
    module: require('./../examples-win/Popup/PopupExample'),
  },
  {
    key: 'KeyboardExtensionExample',
    module: require('./../examples-win/Keyboard/KeyboardExtensionExample'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('react-native/RNTester/js/examples/ScrollView/ScrollViewSimpleExample'),
  },
  {
    key: 'SectionListExample',
    module: require('react-native/RNTester/js/examples/SectionList/SectionListExample'),
  },
  {
    key: 'SwitchExample',
    module: require('react-native/RNTester/js/examples/Switch/SwitchExample'),
  },
  {
    key: 'TextExample',
    module: require('./../examples-win/Text/TextExample'),
  },
  {
    key: 'TextInputExample',
    module: require('./../examples-win/TextInput/TextInputExample'),
  },
  {
    key: 'TouchableExample',
    module: require('react-native/RNTester/js/examples/Touchable/TouchableExample'),
  },
  {
    key: 'TransferProperties',
    module: require('./../examples-win/TransferProperties/TransferPropertiesExample'),
  },
  {
    key: 'TransparentHitTestExample',
    module: require('react-native/RNTester/js/examples/TransparentHitTest/TransparentHitTestExample'),
  },
  {
    key: 'ViewExample',
    module: require('react-native/RNTester/js/examples/View/ViewExample'),
  },
];

const APIExamples: Array<IRNTesterExample> = [
  {
    key: 'KeyboardFocusExample',
    module: require('./../examples-win/Keyboard/KeyboardFocusExample'),
  },
  {
    key: 'AccessibilityExample',
    module: require('react-native/RNTester/js/examples/Accessibility/AccessibilityExample'),
  },
  {
    key: 'AccessibilityExampleWindows',
    module: require('./../examples-win/Accessibility/AccessibilityExampleWindows'),
  },
  {
    key: 'AlertExample',
    module: require('react-native/RNTester/js/examples/Alert/AlertExample')
      .AlertExample,
  },
  {
    key: 'AnimatedExample',
    module: require('react-native/RNTester/js/examples/Animated/AnimatedExample'),
  },
  {
    key: 'AppStateExample',
    module: require('react-native/RNTester/js/examples/AppState/AppStateExample'),
  },
  {
    key: 'BorderExample',
    module: require('react-native/RNTester/js/examples/Border/BorderExample'),
  },
  {
    key: 'BoxShadowExample - windows NotYetImplemented',
    module: require('react-native/RNTester/js/examples/BoxShadow/BoxShadowExample'),
  },
  {
    key: 'ClipboardExample',
    module: require('react-native/RNTester/js/examples/Clipboard/ClipboardExample'),
  },
  {
    key: 'CrashExample',
    module: require('react-native/RNTester/js/examples/Crash/CrashExample'),
  },
  {
    key: 'Dimensions',
    module: require('react-native/RNTester/js/examples/Dimensions/DimensionsExample'),
  },
  {
    key: 'KeyboardExample',
    module: require('./../examples-win/Keyboard/KeyboardExample'),
  },
  {
    key: 'LayoutEventsExample',
    module: require('react-native/RNTester/js/examples/Layout/LayoutEventsExample'),
  },
  {
    key: 'LinkingExample',
    module: require('react-native/RNTester/js/examples/Linking/LinkingExample'),
  },
  {
    key: 'LayoutExample',
    module: require('react-native/RNTester/js/examples/Layout/LayoutExample'),
  },
  {
    key: 'NativeAnimationsExample',
    module: require('react-native/RNTester/js/examples/NativeAnimation/NativeAnimationsExample'),
  },
  {
    key: 'PanResponderExample',
    module: require('react-native/RNTester/js/examples/PanResponder/PanResponderExample'),
  },
  {
    key: 'PointerEventsExample',
    module: require('react-native/RNTester/js/examples/PointerEvents/PointerEventsExample'),
  },
  {
    key: 'RTLExample',
    module: require('react-native/RNTester/js/examples/RTL/RTLExample'),
  },
  {
    key: 'ThemingExample',
    module: require('./../examples-win/Theming/ThemingExample'),
  },
  {
    key: 'WindowsBrushExample',
    module: require('./../examples-win/WindowsBrush/WindowsBrushExample'),
  },
  {
    key: 'TransformExample',
    module: require('react-native/RNTester/js/examples/Transform/TransformExample'),
  },
  {
    key: 'TimerExample',
    module: require('react-native/RNTester/js/examples/Timer/TimerExample'),
  },
  {
    key: 'WebSocketExample',
    module: require('react-native/RNTester/js/examples/WebSocket/WebSocketExample'),
  },
  {
    key: 'TurboModuleExample',
    module: require('react-native/RNTester/js/examples/TurboModule/TurboModuleExample'),
  },
  /* TODO: ? XHRExample requires CameraRoll access
  {
    key: 'XHRExample',
    module: require('react-native/RNTester/js/examples/XHR/XHRExample'),
  },*/
];

const Modules: { [key: string]: IRNTesterModule } = {};

APIExamples.concat(ComponentExamples).forEach((Example: IRNTesterExample) => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIExamples,
  ComponentExamples,
  Modules,
};

export = RNTesterList;
