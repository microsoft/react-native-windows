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
    key: 'ActivityIndicatorExample',
    module: require('react-native/RNTester/js/examples/ActivityIndicator/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('../examples/Button/ButtonExample'),
  },
  /*
  {
    key: 'FlatListExample',
    module: require('react-native/RNTester/js/examples/FlatList/FlatListExample'),
  },
  {
    key: 'ImageExample',
    module: require('react-native/RNTester/js/examples/Image/ImageExample'),
  },
  {
    key: 'MultiColumnExample',
    module: require('react-native/RNTester/js/examples/MultiColumn/MultiColumnExample'),
  }*/
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
  } /*
  {
    key: 'SectionListExample',
    module: require('react-native/RNTester/js/examples/SectionList/SectionListExample'),
  }*/,
  {
    key: 'SwitchExample',
    module: require('react-native/RNTester/js/examples/Switch/SwitchExample'),
  },
  {
    key: 'TouchableExample',
    module: require('react-native/RNTester/js/examples/Touchable/TouchableExample'),
  },
  {
    key: 'TransparentHitTestExample',
    module: require('react-native/RNTester/js/examples/TransparentHitTest/TransparentHitTestExample'),
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
  {
    key: 'ColorGradientWin32Example',
    module: require('../examples/ColorGradientWin32Example'),
  },
];

const APIExamples: Array<RNTesterExample> = [
  {
    key: 'AccessibilityExample',
    module: require('react-native/RNTester/js/examples/Accessibility/AccessibilityExample'),
  },
  {
    key: 'AccessibilityExampleWin32',
    module: require('../../APIExamples/AccessibilityExampleWin32'),
  },
  {
    key: 'AlertExample',
    module: require('../examples/Alert/AlertExample'),
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
    key: 'BoxShadowExample',
    module: require('react-native/RNTester/js/examples/BoxShadow/BoxShadowExample'),
  } /*
  {
    key: 'ClipboardExample',
    module: require('react-native/RNTester/js/examples/Clipboard/ClipboardExample'),
  }*/,
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
  } /*
  {
    key: 'NativeAnimationsExample',
    module: require('react-native/RNTester/js/examples/NativeAnimation/NativeAnimationsExample'),
  }*/,
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
  } /*
  {
    key: 'RTLExample',
    module: require('react-native/RNTester/js/examples/RTL/RTLExample'),
  }*/,
  {
    key: 'ShareExample',
    module: require('react-native/RNTester/js/examples/Share/ShareExample'),
  },
  {
    key: 'TimerExample',
    module: require('react-native/RNTester/js/examples/Timer/TimerExample'),
  },

  {
    key: 'ThemingExample',
    module: require('../../APIExamples/ThemingModuleAPI'),
  },
  {
    key: 'TransformExample',
    module: require('react-native/RNTester/js/examples/Transform/TransformExample'),
  } /*
  {
    key: 'WebSocketExample',
    module: require('react-native/RNTester/js/examples/WebSocket/WebSocketExample'),
  },
  {
    key: 'TurboModuleExample',
    module: require('react-native/RNTester/js/examples/TurboModule/TurboModuleExample'),
  }*/,
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
