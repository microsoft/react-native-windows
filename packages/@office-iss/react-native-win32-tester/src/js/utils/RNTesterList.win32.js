/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

import type {RNTesterModuleInfo} from '../types/RNTesterTypes';

import ReactNativeFeatureFlags from 'react-native/Libraries/ReactNative/ReactNativeFeatureFlags';

const Components: Array<RNTesterModuleInfo> = [
  {
    key: 'AccessibilityExampleWin32',
    module: require('../examples-win32/Accessibility/AccessibilityExampleWin32'),
  },
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
  {
    key: 'ColorGradientWin32Example',
    module: require('../examples-win32/Color/ColorGradientWin32Example'),
  },
  {
    key: 'Dimensions',
    category: 'UI',
    module: require('../examples/Dimensions/DimensionsExample'),
  },
  /*
  {
    key: 'FlatListExampleIndex',
    module: require('../examples/FlatList/FlatListExampleIndex').default,
    category: 'ListView',
    supportsTVOS: true,
  },
  {
    key: 'ImageExample',
    category: 'Basic',
    module: require('../examples/Image/ImageExample'),
  },
  */
  {
    key: 'ImageWin32Test',
    module: require('@office-iss/react-native-win32/Libraries/Image/Tests/ImageWin32Test'),
  },
  /*
  {
    key: 'JSResponderHandlerExample',
    module: require('../examples/JSResponderHandlerExample/JSResponderHandlerExample'),
  },
  {
    key: 'KeyboardAvoidingViewExample',
    module: require('../examples/KeyboardAvoidingView/KeyboardAvoidingViewExample'),
  },
  {
    key: 'ModalExample',
    category: 'UI',
    module: require('../examples/Modal/ModalExample'),
  },
  {
    key: 'NewAppScreenExample',
    module: require('../examples/NewAppScreen/NewAppScreenExample'),
  },
  {
   key: 'NewArchitectureExample',
   category: 'UI',
   module: require('../examples/NewArchitecture/NewArchitectureExample'),
  },
  */
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
    module: require('../examples/SectionList/SectionListIndex'),
  },
  {
    key: 'SwipeableCardExample',
    category: 'UI',
    module: require('../examples/SwipeableCardExample/SwipeableCardExample'),
  }*/ {
    key: 'SwitchExample',
    category: 'UI',
    module: require('../examples/Switch/SwitchExample'),
  },
  {
    key: 'TextInputs with key prop',
    module: require('../examples/TextInput/TextInputKeyProp'),
  },
  {
    key: 'TextInputExample',
    category: 'Basic',
    module: require('@office-iss/react-native-win32/Libraries/Components/TextInput/Tests/TextInputTest'),
  },
  {
    key: 'TextWin32Test',
    module: require('@office-iss/react-native-win32/Libraries/Components/Text/Tests/TextWin32Test'),
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
];

const APIs: Array<RNTesterModuleInfo> = [
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
    module: require('../examples/Alert/AlertExample').default,
  },
  {
    key: 'AnimatedIndex',
    category: 'UI',
    module: require('../examples/Animated/AnimatedIndex').default,
  },
  {
    key: 'Animation - GratuitousAnimation',
    category: 'UI',
    module: require('../examples/AnimatedGratuitousApp/AnExApp'),
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
    key: 'Keyboard',
    category: 'Basic',
    module: require('../examples/Keyboard/KeyboardExample').default,
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
  },
  {
    key: 'XHRExample',
    category: 'Basic',
    module: require('../examples/XHR/XHRExample'),
  },
  {
  }*/,
  {
    key: 'TurboModuleExample',
    category: 'Basic',
    module: require('../examples/TurboModule/TurboModuleExample'),
  },
];

if (ReactNativeFeatureFlags.shouldEmitW3CPointerEvents()) {
  APIs.push({
    key: 'W3C PointerEvents',
    category: 'Experimental',
    module: require('../examples/Experimental/W3CPointerEventsExample').default,
  });
}

const Modules: any = {};

APIs.concat(Components).forEach(Example => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIs,
  Components,
  Modules,
};

module.exports = RNTesterList;
