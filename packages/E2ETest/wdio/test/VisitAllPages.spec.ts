/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';

let pages = [
  '<ActivityIndicator>',
  '<Button>',
  //  'Custom Views',
  '<DatePicker>',
  'Fast Path Texts',
  '<FlatList>',
  '<Flyout>',
  '<Glyph> UWP',
  '<Image>',
  //  '<FlatList> - MultiColumn',
  'New App Screen',
  '<Picker>',
  '<PickerWindows>',
  '<Pressable>',
  '<Popup>',
  'Keyboard extension Example',
  '<ScrollViewSimpleExample>',
  //  '<SectionList>',
  '<Switch>',
  '<Text>',
  '<TextInput>',
  //'<Touchable*> and onPress',
  '<TransferProperties>',
  '<TransparentHitTestExample>',
  '<View>',
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
  'Box Shadow',
  'Clipboard',
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
  //  '<LegacyControlStyleTest>',
  //  '<LegacyTransformTest>',
  //  '<LegacyTextInputTest>',
  //  '<LegacyLoginTest>',
  //  '<LegacyDirectManipulationTest>',
  //  '<LegacyImageTest>',
  //  '<LegacyAccessibilityTest>',
];

describe('VisitAllPagesTest', () => {
  pages.forEach(function(page) {
    it(page, () => {
      console.log('loading page ' + page);
      HomePage.goToTestPage(page);
      HomePage.backToHomePage();
    });
  });
});
