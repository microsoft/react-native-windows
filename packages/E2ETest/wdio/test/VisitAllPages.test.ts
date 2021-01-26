/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import { goToComponentExample, goToApiExample } from './Helpers';

describe('VisitAllPagesTest', () => {
  const componentExamples = [
    'ActivityIndicator',
    'Button',
    'DatePicker',
    'Fast Path Texts',
    'FlatList',
    'Flyout',
    'Glyph UWP',
    'Image',
    //  'FlatList - MultiColumn',
    'New App Screen',
    'PickerWindows',
    'Pressable',
    'Popup',
    'ScrollViewSimpleExample',
    //  'SectionList',
    'Switch',
    'Text',
    'TextInput',
    //'Touchable* and onPress',
    'TransferProperties',
    'TransparentHitTestExample',
    'View',
    //  'LegacyControlStyleTest',
    //  'LegacyTextInputTest',
    //  'LegacyLoginTest',
    //  'LegacyImageTest',
  ];

  const apiExamples = [
    'Keyboard Focus Example',
    'Accessibility',
    'AccessibilityInfo',
    'Accessibility Windows',
    'AsyncStorage Windows',
    'Alerts',
    'Animated - Examples',
    'Animated - Gratuitous App',
    'Appearance',
    'AppState',
    'Border',
    'Crash',
    'DevSettings',
    'Dimensions',
    'Keyboard',
    'Layout Events',
    'Linking',
    'Layout - Flexbox',
    'Mouse Events',
    'Native Animated Example',
    'PanResponder Sample',
    'PlatformColor',
    'Pointer Events',
    'RTLExample',
    'Share',
    'Timers',
    'WebSocket',
    'Transforms',
  ];

  for (const component of componentExamples) {
    it(component, async () => await goToComponentExample(component));
  }

  for (const api of apiExamples) {
    it(api, async () => await goToApiExample(api));
  }
});
