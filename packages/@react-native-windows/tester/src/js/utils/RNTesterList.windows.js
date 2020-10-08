/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

import type {RNTesterExample} from '../types/RNTesterTypes';

const ComponentExamples: Array<RNTesterExample> = [
  {
    key: 'ActivityIndicatorExample',
    module: require('../examples/ActivityIndicator/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('../examples/Button/ButtonExample'),
  },
  {
    key: 'CustomViewExample',
    module: require('../examples-win/CustomView/CustomViewExample'),
  },
  {
    key: 'DatePickerExample',
    module: require('../examples-win/DatePicker/DatePickerExample'),
  },
  {
    key: 'FastTextExample',
    module: require('../examples-win/FastText/FastTextExample'),
  },
  {
    key: 'FlatListExample',
    module: require('../examples/FlatList/FlatListExample'),
  },
  {
    key: 'FlyoutExample',
    module: require('../examples-win/Flyout/FlyoutExample'),
  },
  {
    key: 'GlyphExample',
    module: require('../examples-win/Glyph/GlyphExample'),
  },
  {
    key: 'ImageExample',
    module: require('../examples/Image/ImageExample'),
  },
  {
    key: 'MultiColumnExample',
    module: require('../examples/MultiColumn/MultiColumnExample'),
  },
  {
    key: 'NewAppScreenExample',
    module: require('../examples/NewAppScreen/NewAppScreenExample'),
  },
  {
    key: 'PickerExample',
    module: require('../examples/Picker/PickerExample'),
  },
  {
    key: 'PickerWindowsExample',
    module: require('../examples-win/Picker/PickerWindowsExample'),
  },
  {
    key: 'PressableExample',
    module: require('../examples/Pressable/PressableExample'),
  },
  {
    key: 'PopupExample',
    module: require('../examples-win/Popup/PopupExample'),
  },
  {
    key: 'KeyboardExtensionExample',
    module: require('../examples-win/Keyboard/KeyboardExtensionExample'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('../examples/ScrollView/ScrollViewSimpleExample'),
  },
  {
    key: 'SectionListExample',
    module: require('../examples/SectionList/SectionListExample'),
  },
  {
    key: 'SwitchExample',
    module: require('../examples/Switch/SwitchExample'),
  },
  {
    key: 'TextExample',
    module: require('../examples-win/Text/TextExample'),
  },
  {
    key: 'TextInputExample',
    module: require('../examples/TextInput/TextInputExample'),
  },
  {
    key: 'TouchableExample',
    module: require('../examples/Touchable/TouchableExample'),
  },
  {
    key: 'TransferProperties',
    module: require('../examples-win/TransferProperties/TransferPropertiesExample'),
  },
  {
    key: 'TransparentHitTestExample',
    module: require('../examples/TransparentHitTest/TransparentHitTestExample'),
  },
  {
    key: 'ViewExample',
    module: require('../examples/View/ViewExample'),
  },
];

const APIExamples: Array<RNTesterExample> = [
  {
    key: 'KeyboardFocusExample',
    module: require('../examples-win/Keyboard/KeyboardFocusExample'),
  },
  {
    key: 'AccessibilityExample',
    module: require('../examples/Accessibility/AccessibilityExample'),
  },
  {
    key: 'AccessibilityExampleWindows',
    module: require('../examples-win/Accessibility/AccessibilityExampleWindows'),
  },
  {
    key: 'AccessibilityInfo',
    module: require('../examples-win/Accessibility/AccessibilityInfoExample'),
  },
  {
    key: 'AsyncStorageExampleWindows',
    module: require('../examples-win/AsyncStorage/AsyncStorageExampleWindows'),
  },
  {
    key: 'AlertExample',
    module: require('../examples/Alert/AlertExample'),
  },
  {
    key: 'AnimatedExample',
    module: require('../examples/Animated/AnimatedExample'),
  },
  {
    key: 'Animation - GratuitousAnimation',
    module: require('../examples/Animated/AnimatedGratuitousApp/AnExApp'),
  },
  {
    key: 'AppearanceExample',
    module: require('../examples/Appearance/AppearanceExample'),
  },
  {
    key: 'AppStateExample',
    module: require('../examples/AppState/AppStateExample'),
  },
  {
    key: 'BorderExample',
    module: require('../examples/Border/BorderExample'),
  },
  {
    key: 'BoxShadowExample - Windows NotYetImplemented',
    module: require('../examples/BoxShadow/BoxShadowExample'),
  },
  {
    key: 'ClipboardExample',
    module: require('../examples/Clipboard/ClipboardExample'),
  },
  {
    key: 'CrashExample',
    module: require('../examples/Crash/CrashExample'),
  },
  {
    key: 'DevSettings',
    module: require('../examples/DevSettings/DevSettingsExample'),
  },
  {
    key: 'Dimensions',
    module: require('../examples/Dimensions/DimensionsExample'),
  },
  {
    key: 'KeyboardExample',
    module: require('../examples-win/Keyboard/KeyboardExample'),
  },
  {
    key: 'LayoutEventsExample',
    module: require('../examples/Layout/LayoutEventsExample'),
  },
  {
    key: 'LinkingExample',
    module: require('../examples/Linking/LinkingExample'),
  },
  {
    key: 'LayoutExample',
    module: require('../examples/Layout/LayoutExample'),
  },
  {
    key: 'MouseExample',
    module: require('../examples-win/Mouse/MouseExample'),
  },
  {
    key: 'NativeAnimationsExample',
    module: require('../examples/NativeAnimation/NativeAnimationsExample'),
  },
  {
    key: 'PanResponderExample',
    module: require('../examples/PanResponder/PanResponderExample'),
  },
  {
    key: 'PlatformColorExample',
    module: require('../examples/PlatformColor/PlatformColorExample'),
  },
  {
    key: 'PointerEventsExample',
    module: require('../examples/PointerEvents/PointerEventsExample'),
  },
  {
    key: 'RTLExample',
    module: require('../examples/RTL/RTLExample'),
  },
  {
    key: 'ShareExample',
    module: require('../examples/Share/ShareExample'),
  },
  {
    key: 'TimerExample',
    module: require('../examples/Timer/TimerExample'),
  },
  {
    key: 'TransformExample',
    module: require('../examples/Transform/TransformExample'),
  },
  {
    key: 'WebSocketExample',
    module: require('../examples/WebSocket/WebSocketExample'),
  },
  {
    key: 'LegacyControlStyleTest',
    module: require('../examples-win/LegacyTests/ControlStyleTestPage'),
  },
  {
    key: 'LegacyTransformTest',
    module: require('../examples-win/LegacyTests/TransformTestPage'),
  },
  {
    key: 'LegacyTextInputTest',
    module: require('../examples-win/LegacyTests/TextInputTestPage'),
  },
  {
    key: 'LegacyLoginTest',
    module: require('../examples-win/LegacyTests/LoginTestPage'),
  },
  {
    key: 'LegacyDirectManipulationTest',
    module: require('../examples-win/LegacyTests/DirectManipulationTestPage'),
  },
  {
    key: 'LegacyImageTest',
    module: require('../examples-win/LegacyTests/ImageTestPage'),
  },
  {
    key: 'LegacyAccessibilityTest',
    module: require('../examples-win/LegacyTests/AccessibilityTestPage'),
  },
  // TODO:  TurboModuleExample crashes the app if web debugging is turned on
  //  {
  //    key: 'TurboModuleExample',
  //    module: require('../examples/TurboModule/TurboModuleExample'),
  //  },
  /* TODO: ? XHRExample requires CameraRoll access
  {
    key: 'XHRExample',
    module: require('../examples/XHR/XHRExample'),
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
