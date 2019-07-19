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
    key: 'CustomViewExample',
    module: require('./CustomViewExample.uwp'),
  },
  {
    key: 'DatePickerExample',
    module: require('./DatePickerExample.uwp'),
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
    module: require('./FlyoutExample.uwp'),
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
    key: 'LayoutEventsExample',
    module: require('react-native/RNTester/js/LayoutEventsExample'),
  },
  {
    key: 'PickerUWPExample',
    module: require('./PickerUWPExample'),
  },
  {
    key: 'PopupExample',
    module: require('./PopupExample.uwp'),
  },
  {
    key: 'KeyboardExtensionExample',
    module: require('./KeyboardExtensionExample.uwp'),
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
    module: require('./TextExample.uwp').default,
  },
  {
    key: 'TextInputExample',
    module: require('./TextInputExample.uwp'),
  },
  {
    key: 'TouchableExample',
    module: require('react-native/RNTester/js/TouchableExample'),
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
    module: require('./KeyboardFocusExample.uwp'),
  },
  {
    key: 'AccessibilityExample',
    module: require('./AccessibilityExample'),
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
    key: 'ThemingExample',
    module: require('./ThemingExample'),
  },
  {
    key: 'BorderExample',
    module: require('react-native/RNTester/js/BorderExample'),
  },
  {
    key: 'BoxShadowExample - UWP NotYetImplemented',
    module: require('react-native/RNTester/js/BoxShadowExample'),
  },
  {
    key: 'ClipboardExample',
    module: require('react-native/RNTester/js/ClipboardExample'),
  },
  {
    key: 'Dimensions',
    module: require('react-native/RNTester/js/DimensionsExample'),
  },
  {
    key: 'GeolocationExample',
    module: require('react-native/RNTester/js/GeolocationExample'),
  },
  {
    key: 'KeyboardExample',
    module: require('./KeyboardExample'),
  },
  {
    key: 'LayoutExample',
    module: require('react-native/RNTester/js/LayoutExample'),
  },
  {
    key: 'LinkingExample',
    module: require('react-native/RNTester/js/LinkingExample'),
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
