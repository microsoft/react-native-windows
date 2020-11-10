/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';
import * as React from 'react';

import type {RNTesterExample} from '../types/RNTesterTypes';

const ComponentExamples: Array<RNTesterExample> = [
  {
    key: 'ActivityIndicatorExample',
    module: require('react-native/RNTester/js/examples/ActivityIndicator/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('react-native/RNTester/js/examples/Button/ButtonExample'),
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
    key: 'PressableExample',
    module: require('../examples/Pressable/PressableExample'),
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
    module: require('react-native/RNTester/js/examples/TextInput/TextInputExample'),
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

const APIExamples: Array<RNTesterExample> = [
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
    key: 'AccessibilityInfo',
    module: require('react-native/RNTester/js/examples/Accessibility/AccessibilityInfoExample'),
  },
  {
    key: 'AsyncStorageExampleWindows',
    module: require('./../examples-win/AsyncStorage/AsyncStorageExampleWindows'),
  },
  {
    key: 'AlertExample',
    module: {
      title: require('react-native/RNTester/js/examples/Alert/AlertExample')
        .AlertExample.title,
      description: require('react-native/RNTester/js/examples/Alert/AlertExample')
        .AlertExample.description,
      examples: [
        {
          title: require('react-native/RNTester/js/examples/Alert/AlertExample')
            .AlertExample.title,
          render: () => {
            return React.createElement(
              require('react-native/RNTester/js/examples/Alert/AlertExample')
                .AlertExample,
            );
          },
        },
      ],
    },
  },
  {
    key: 'AnimatedExample',
    module: require('react-native/RNTester/js/examples/Animated/AnimatedExample'),
  },
  {
    key: 'Animation - GratuitousAnimation',
    module: require('react-native/RNTester/js/examples/Animated/AnimatedGratuitousApp/AnExApp'),
  },
  {
    key: 'AppearanceExample',
    module: require('react-native/RNTester/js/examples/Appearance/AppearanceExample'),
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
    key: 'BoxShadowExample - Windows NotYetImplemented',
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
    key: 'DevSettings',
    module: require('react-native/RNTester/js/examples/DevSettings/DevSettingsExample'),
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
    key: 'MouseExample',
    module: require('./../examples-win/Mouse/MouseExample'),
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
    key: 'PlatformColorExample',
    module: require('../examples/PlatformColor/PlatformColorExample'),
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
    key: 'ShareExample',
    module: require('react-native/RNTester/js/examples/Share/ShareExample'),
  },
  {
    key: 'TimerExample',
    module: require('react-native/RNTester/js/examples/Timer/TimerExample'),
  },
  {
    key: 'TransformExample',
    module: require('react-native/RNTester/js/examples/Transform/TransformExample'),
  },
  {
    key: 'WebSocketExample',
    module: require('react-native/RNTester/js/examples/WebSocket/WebSocketExample'),
  },
  {
    key: 'LegacyControlStyleTest',
    module: require('react-native/RNTester/js/examples-win/LegacyTests/ControlStyleTestPage'),
  },
  {
    key: 'LegacyTransformTest',
    module: require('react-native/RNTester/js/examples-win/LegacyTests/TransformTestPage'),
  },
  {
    key: 'LegacyTextInputTest',
    module: require('react-native/RNTester/js/examples-win/LegacyTests/TextInputTestPage'),
  },
  {
    key: 'LegacyLoginTest',
    module: require('react-native/RNTester/js/examples-win/LegacyTests/LoginTestPage'),
  },
  {
    key: 'LegacyDirectManipulationTest',
    module: require('react-native/RNTester/js/examples-win/LegacyTests/DirectManipulationTestPage'),
  },
  {
    key: 'LegacyImageTest',
    module: require('react-native/RNTester/js/examples-win/LegacyTests/ImageTestPage'),
  },
  {
    key: 'LegacyAccessibilityTest',
    module: require('react-native/RNTester/js/examples-win/LegacyTests/AccessibilityTestPage'),
  },
  // TODO:  TurboModuleExample crashes the app if web debugging is turned on
  //  {
  //    key: 'TurboModuleExample',
  //    module: require('react-native/RNTester/js/examples/TurboModule/TurboModuleExample'),
  //  },
  /* TODO: ? XHRExample requires CameraRoll access
  {
    key: 'XHRExample',
    module: require('react-native/RNTester/js/examples/XHR/XHRExample'),
  },*/
];

const Modules: any = {};

APIExamples.concat(ComponentExamples).forEach(Example => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIExamples,
  ComponentExamples,
  Modules,
};

module.exports = RNTesterList;
