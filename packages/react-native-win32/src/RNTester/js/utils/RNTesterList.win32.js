/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

import type {RNTesterExample} from '../types/RNTesterTypes';

const ComponentExamples: Array<RNTesterExample> = [
  {
    key: 'ButtonExample',
    module: require('../examples/Button/ButtonExample'),
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
    key: 'PressableExample',
    module: require('../examples/Pressable/PressableExample'),
  },
  {
    key: 'TouchableWin32Example',
    module: require('../../../Libraries/Components/Touchable/Tests/TouchableWin32Test'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('react-native/RNTester/js/examples/ScrollView/ScrollViewSimpleExample'),
  },
  {
    key: 'TouchableExample',
    module: require('react-native/RNTester/js/examples/Touchable/TouchableExample'),
  },
  {
    key: 'ViewExample',
    module: require('react-native/RNTester/js/examples/View/ViewExample'),
  },
  {
    key: 'ViewWin32Test',
    module: require('../../../Libraries/Components/View/Tests/ViewWin32Test'),
  },
  {
    key: 'TextInputExample',
    module: require('../../../Libraries/Components/TextInput/Tests/TextInputTest'),
  },
  {
    key: 'ImageWin32Test',
    module: require('../../../Libraries/Image/Tests/ImageWin32Test'),
  },
];

const APIExamples: Array<RNTesterExample> = [
  {
    key: 'AccessibilityExampleWin32',
    module: require('../../APIExamples/AccessibilityExampleWin32'),
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
    key: 'LayoutExample',
    module: require('react-native/RNTester/js/examples/Layout/LayoutExample'),
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
    key: 'ThemingExample',
    module: require('../../APIExamples/ThemingModuleAPI'),
  },
];

const Modules: {...} = {};

APIExamples.concat(ComponentExamples).forEach(Example => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIExamples,
  ComponentExamples,
  Modules,
};

module.exports = RNTesterList;
