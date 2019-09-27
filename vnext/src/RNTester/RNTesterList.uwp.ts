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
    module: require('react-native/RNTester/js/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('react-native/RNTester/js/ButtonExample'),
  },
  {
    key: 'CheckBoxExample',
    module: require('react-native/RNTester/js/CheckBoxExample'),
  },
  {
    key: 'CustomViewExample',
    module: require('./CustomViewExample.windows'),
  },
  {
    key: 'DatePickerExample',
    module: require('./DatePickerExample.windows'),
  },
  {
    key: 'FastTextExample',
    module: require('./FastTextExample'),
  },
  {
    key: 'FlatListExample',
    module: require('react-native/RNTester/js/FlatListExample'),
  },
  {
    key: 'FlyoutExample',
    module: require('./FlyoutExample.windows'),
  },
  {
    key: 'GlyphExample',
    module: require('./GlyphExample'),
  },
  {
    key: 'ImageExample',
    module: require('react-native/RNTester/js/ImageExample'),
  },
  {
    key: 'MultiColumnExample',
    module: require('react-native/RNTester/js/MultiColumnExample'),
  },
  {
    key: 'NewAppScreenExample',
    module: require('react-native/RNTester/js/NewAppScreenExample'),
  },
  {
    key: 'PickerExample',
    module: require('react-native/RNTester/js/PickerExample'),
  },
  {
    key: 'PickerWindowsExample',
    module: require('./PickerWindowsExample'),
  },
  {
    key: 'PopupExample',
    module: require('./PopupExample.windows'),
  },
  {
    key: 'KeyboardExtensionExample',
    module: require('./KeyboardExtensionExample.windows'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('react-native/RNTester/js/ScrollViewSimpleExample'),
  },
  {
    key: 'SectionListExample',
    module: require('react-native/RNTester/js/SectionListExample'),
  },
  {
    key: 'SwitchExample',
    module: require('react-native/RNTester/js/SwitchExample'),
  },
  {
    key: 'TextExample',
    module: require('./TextExample.windows').default,
  },
  {
    key: 'TextInputExample',
    module: require('./TextInputExample.windows'),
  },
  {
    key: 'TouchableExample',
    module: require('react-native/RNTester/js/TouchableExample'),
  },
  {
    key: 'TransferProperties',
    module: require('./TransferProperties'),
  },
  {
    key: 'TransparentHitTestExample',
    module: require('react-native/RNTester/js/TransparentHitTestExample'),
  },
  {
    key: 'ViewExample',
    module: require('react-native/RNTester/js/ViewExample'),
  },
  /* TODO: Not enough of WebView is implemented to load the Example
  {
    key: 'WebViewExample',
    module: require('react-native/RNTester/js/WebViewExample'),
  },*/
];

const APIExamples: Array<IRNTesterExample> = [
  {
    key: 'KeyboardFocusExample',
    module: require('./KeyboardFocusExample.windows'),
  },
  {
    key: 'AccessibilityExample',
    module: require('react-native/RNTester/js/AccessibilityExample'),
  },
  {
    key: 'AccessibilityExampleWindows',
    module: require('./AccessibilityExampleWindows'),
  },
  {
    key: 'AlertExample',
    module: require('react-native/RNTester/js/AlertExample').AlertExample,
  },
  {
    key: 'AnimatedExample',
    module: require('react-native/RNTester/js/AnimatedExample'),
  },
  {
    key: 'AppStateExample',
    module: require('react-native/RNTester/js/AppStateExample'),
  },
  {
    key: 'BorderExample',
    module: require('react-native/RNTester/js/BorderExample'),
  },
  {
    key: 'BoxShadowExample - windows NotYetImplemented',
    module: require('react-native/RNTester/js/BoxShadowExample'),
  },
  {
    key: 'ClipboardExample',
    module: require('react-native/RNTester/js/ClipboardExample'),
  },
  {
    key: 'CrashExample',
    module: require('react-native/RNTester/js/CrashExample'),
  },
  {
    key: 'Dimensions',
    module: require('react-native/RNTester/js/DimensionsExample'),
  },
  {
    key: 'KeyboardExample',
    module: require('./KeyboardExample'),
  },
  {
    key: 'LayoutEventsExample',
    module: require('react-native/RNTester/js/LayoutEventsExample'),
  },
  {
    key: 'LinkingExample',
    module: require('react-native/RNTester/js/LinkingExample'),
  },
  {
    key: 'LayoutExample',
    module: require('react-native/RNTester/js/LayoutExample'),
  },
  {
    key: 'NativeAnimationsExample',
    module: require('react-native/RNTester/js/NativeAnimationsExample'),
  },
  {
    key: 'PanResponderExample',
    module: require('react-native/RNTester/js/PanResponderExample'),
  },
  {
    key: 'PointerEventsExample',
    module: require('react-native/RNTester/js/PointerEventsExample'),
  },
  {
    key: 'RTLExample',
    module: require('react-native/RNTester/js/RTLExample'),
  },
  {
    key: 'ThemingExample',
    module: require('./ThemingExample'),
  },
  {
    key: 'TransformExample',
    module: require('react-native/RNTester/js/TransformExample'),
  },
  {
    key: 'TimerExample',
    module: require('react-native/RNTester/js/TimerExample'),
  },
  {
    key: 'WebSocketExample',
    module: require('react-native/RNTester/js/WebSocketExample'),
  },
  /* TODO: ? XHRExample requires CameraRoll access
  {
    key: 'XHRExample',
    module: require('react-native/RNTester/js/XHRExample'),
  },*/
];

const Modules: {[key: string]: IRNTesterModule} = {};

APIExamples.concat(ComponentExamples).forEach((Example: IRNTesterExample) => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIExamples,
  ComponentExamples,
  Modules,
};

export default RNTesterList;
