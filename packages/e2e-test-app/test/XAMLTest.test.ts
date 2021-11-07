/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  await goToComponentExample('XAML');
});

describe('XamlTest', () => {
  test('Layout', async () => {
    const dump = await dumpVisualTree('ReactNativeXAMLRoot');
    expect(dump).toMatchSnapshot();
  });
  test('MenuFlyout IsOpen', async () => {
    const tappableTextBlock = await app.findElementByTestID(
      'tappableTextBlock',
    );

    await tappableTextBlock.click();
    await app.findElementByTestID('MenuFlyout');

    const dump = await dumpVisualTree('MenuFlyout');
    expect(dump).toMatchSnapshot('ReactNativeXAML-MenuFlyout');

    const menuOption2 = await app.findElementByTestID('menuOption2');
    await menuOption2.click();
    const menuOption2Text = 'menu option 2 clicked';
    await app.waitUntil(async () => {
      const text = await tappableTextBlock.getText();
      return text === menuOption2Text;
    });
    const text = await tappableTextBlock.getText();
    expect(text).toBe(menuOption2Text);
  });
});
