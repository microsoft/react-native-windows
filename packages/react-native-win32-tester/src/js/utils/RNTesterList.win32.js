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
    module: require('../examples/ActivityIndicator/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('../examples/Button/ButtonExample'),
  },
  /*
  {
    key: 'FlatListExample',
    module: require('../examples/FlatList/FlatListExample'),
  },
  {
    key: 'ImageExample',
    module: require('../examples/Image/ImageExample'),
  },
  {
    key: 'MultiColumnExample',
    module: require('../examples/MultiColumn/MultiColumnExample'),
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
    module: require('@office-iss/react-native-win32/Libraries/Components/Touchable/Tests/TouchableWin32Test'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('../examples/ScrollView/ScrollViewSimpleExample'),
  } /*
  {
    key: 'SectionListExample',
    module: require('../examples/SectionList/SectionListExample'),
  }*/,
  {
    key: 'SwitchExample',
    module: require('../examples/Switch/SwitchExample'),
  },
  {
    key: 'TouchableExample',
    module: require('../examples/Touchable/TouchableExample'),
  },
  {
    key: 'TransparentHitTestExample',
    module: require('../examples/TransparentHitTest/TransparentHitTestExample'),
  },
  {
    key: 'ViewExample',
    module: require('../examples/View/ViewExample'),
  },
  {
    key: 'ViewWin32Test',
    module: require('@office-iss/react-native-win32/Libraries/Components/View/Tests/ViewWin32Test'),
  },
  {
    key: 'TextInputExample',
    module: require('@office-iss/react-native-win32/Libraries/Components/TextInput/Tests/TextInputTest'),
  },
  {
    key: 'ImageWin32Test',
    module: require('@office-iss/react-native-win32/Libraries/Image/Tests/ImageWin32Test'),
  },
  {
    key: 'ColorGradientWin32Example',
    module: require('../examples-win32/Color/ColorGradientWin32Example'),
  },
];

const APIExamples: Array<RNTesterExample> = [
  {
    key: 'AccessibilityExample',
    module: require('../examples/Accessibility/AccessibilityExample'),
  },
  {
    key: 'AccessibilityExampleWin32',
    module: require('../examples-win32/Accessibility/AccessibilityExampleWin32'),
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
    key: 'BoxShadowExample',
    module: require('../examples/BoxShadow/BoxShadowExample'),
  } /*
  {
    key: 'ClipboardExample',
    module: require('../examples/Clipboard/ClipboardExample'),
  }*/,
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
  } /*
  {
    key: 'NativeAnimationsExample',
    module: require('../examples/NativeAnimation/NativeAnimationsExample'),
  }*/,
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
  } /*
  {
    key: 'RTLExample',
    module: require('../examples/RTL/RTLExample'),
  }*/,
  {
    key: 'ShareExample',
    module: require('../examples/Share/ShareExample'),
  },
  {
    key: 'TimerExample',
    module: require('../examples/Timer/TimerExample'),
  },

  {
    key: 'ThemingExample',
    module: require('../examples-win32/Theming/ThemingModuleAPI'),
  },
  {
    key: 'TransformExample',
    module: require('../examples/Transform/TransformExample'),
  } /*
  {
    key: 'WebSocketExample',
    module: require('../examples/WebSocket/WebSocketExample'),
  },
  {
    key: 'TurboModuleExample',
    module: require('../examples/TurboModule/TurboModuleExample'),
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
