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
    category: 'UI',
    module: require('../examples/ActivityIndicator/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    category: 'UI',
    module: require('../examples/Button/ButtonExample'),
  },
  /*
  {
    key: 'FlatListExample',
    category: 'ListView',
    module: require('../examples/FlatList/FlatListExample'),
  },
  {
    key: 'ImageExample',
    category: 'Basic',
    module: require('../examples/Image/ImageExample'),
  },
  {
    key: 'MultiColumnExample',
    category: 'ListView',
    module: require('../examples/MultiColumn/MultiColumnExample'),
  }*/
  {
    key: 'NewAppScreenExample',
    module: require('../examples/NewAppScreen/NewAppScreenExample'),
  },
  {
    key: 'PressableExample',
    category: 'UI',
    module: require('../examples-win32/Pressable/PressableExample'),
  },
  /*
  {
    key: 'TouchableWin32Example',
    module: require('@office-iss/react-native-win32/Libraries/Components/Touchable/Tests/TouchableWin32Test'),
  },
  {
    key: 'ScrollViewSimpleExample',
    category: 'Basic',
    module: require('../examples/ScrollView/ScrollViewSimpleExample'),
  } /*
  {
    key: 'SectionListExample',
    category: 'ListView',
    module: require('../examples/SectionList/SectionListExample'),
  }*/ {
    key: 'SwitchExample',
    module: require('../examples/Switch/SwitchExample'),
  },
  {
    key: 'TouchableExample',
    category: 'UI',
    module: require('../examples/Touchable/TouchableExample'),
  },
  {
    key: 'TransparentHitTestExample',
    module: require('../examples/TransparentHitTest/TransparentHitTestExample'),
  },
  {
    key: 'ViewExample',
    category: 'Basic',
    module: require('../examples/View/ViewExample'),
  },
  {
    key: 'ViewWin32Test',
    module: require('@office-iss/react-native-win32/Libraries/Components/View/Tests/ViewWin32Test'),
  },
  {
    key: 'TextWin32Example',
    module: require('@office-iss/react-native-win32/Libraries/Components/Text/Tests/TextWin32Test'),
  },
  {
    key: 'TextInputExample',
    category: 'Basic',
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
    category: 'Basic',
    module: require('../examples/Accessibility/AccessibilityExample'),
  },
  {
    key: 'AccessibilityExampleWin32',
    module: require('../examples-win32/Accessibility/AccessibilityExampleWin32'),
  },
  {
    key: 'AlertExample',
    category: 'UI',
    module: require('../examples/Alert/AlertExample'),
  },
  {
    key: 'AnimatedExample',
    category: 'UI',
    module: require('../examples/Animated/AnimatedExample'),
  },
  {
    key: 'Animation - GratuitousAnimation',
    category: 'UI',
    module: require('../examples/Animated/AnimatedGratuitousApp/AnExApp'),
  },
  {
    key: 'AppearanceExample',
    category: 'UI',
    module: require('../examples/Appearance/AppearanceExample'),
  },
  {
    key: 'AppStateExample',
    category: 'Basic',
    module: require('../examples/AppState/AppStateExample'),
  },
  {
    key: 'BorderExample',
    category: 'UI',
    module: require('../examples/Border/BorderExample'),
  },
  {
    key: 'CrashExample',
    category: 'Basic',
    module: require('../examples/Crash/CrashExample'),
  },
  {
    key: 'DevSettings',
    category: 'Basic',
    module: require('../examples/DevSettings/DevSettingsExample'),
  },
  {
    key: 'Dimensions',
    category: 'UI',
    module: require('../examples/Dimensions/DimensionsExample'),
  },
  {
    key: 'LayoutEventsExample',
    category: 'UI',
    module: require('../examples/Layout/LayoutEventsExample'),
  },
  {
    key: 'LinkingExample',
    category: 'Basic',
    module: require('../examples/Linking/LinkingExample'),
  },
  {
    key: 'LayoutExample',
    category: 'UI',
    module: require('../examples/Layout/LayoutExample'),
  } /*
  {
    key: 'NativeAnimationsExample',
    category: 'UI',
    module: require('../examples/NativeAnimation/NativeAnimationsExample'),
  }*/,
  {
    key: 'PanResponderExample',
    category: 'Basic',
    module: require('../examples/PanResponder/PanResponderExample'),
  },
  {
    key: 'PlatformColorExample',
    category: 'UI',
    module: require('../examples/PlatformColor/PlatformColorExample'),
  },
  {
    key: 'PointerEventsExample',
    category: 'Basic',
    module: require('../examples/PointerEvents/PointerEventsExample'),
  } /*
  {
    key: 'RTLExample',
    category: 'Basic',
    module: require('../examples/RTL/RTLExample'),
  }*/,
  {
    key: 'ShareExample',
    category: 'Basic',
    module: require('../examples/Share/ShareExample'),
  },
  {
    key: 'TimerExample',
    category: 'UI',
    module: require('../examples/Timer/TimerExample'),
  },

  {
    key: 'ThemingExample',
    module: require('../examples-win32/Theming/ThemingModuleAPI'),
  },
  {
    key: 'TransformExample',
    category: 'UI',
    module: require('../examples/Transform/TransformExample'),
  } /*
  {
    key: 'WebSocketExample',
    category: 'Basic',
    module: require('../examples/WebSocket/WebSocketExample'),
  }*/,
];

if (global.__turboModuleProxy) {
  APIExamples.push({
    key: 'TurboModuleExample',
    category: 'Basic',
    module: require('../examples/TurboModule/TurboModuleExample'),
  });
}

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
