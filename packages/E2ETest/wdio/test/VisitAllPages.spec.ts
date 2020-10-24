/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import HomePage from '../pages/HomePage';

import { By } from '../pages/BasePage';

const componentExamples = [
  'ActivityIndicator',
  'Button',
  //  'Custom Views',
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
  //  'LegacyTransformTest',
  //  'LegacyTextInputTest',
  //  'LegacyLoginTest',
  //  'LegacyDirectManipulationTest',
  //  'LegacyImageTest',
  //  'LegacyAccessibilityTest',
];

const apiExamples = [
  'Keyboard Focus Example',
  'Accessibility',
  'AccessibilityInfo',
  'Accessibility Windows',
  'AsyncStorage Windows',
  'Alert',
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

describe('VisitAllPagesTest', () => {
  componentExamples.forEach(ex => visitTestPage(ex, 'component'));
  apiExamples.forEach(ex => visitTestPage(ex, 'api'));
});

function visitTestPage(name: string, type: 'api' | 'component') {
  it(name, () => {
    console.log('loading page ' + name);

    if (type === 'api') {
      HomePage.waitForElementLoaded('apis-tab');
      By('apis-tab').click();
    } else {
      HomePage.waitForElementLoaded('components-tab');
      By('components-tab').click();
    }

    HomePage.goToTestPage(name);
    HomePage.backToHomePage();
  });
}
