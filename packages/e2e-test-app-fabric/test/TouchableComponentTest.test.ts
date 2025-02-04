/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Touchable* and onPress');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Touchable Tests', () => {
  test('Touchables can contain a Text component', async () => {
    const component = await app.findElementByTestID(
      'touchable_highlight_text_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_highlight_text_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can contain an Image component', async () => {
    const component = await app.findElementByTestID(
      'touchable_highlight_image_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_highlight_image_button');
    expect(dump).toMatchSnapshot();
  });
  test('TouchableWithoutFeedback components should not give visual feedback', async () => {
    const component = await app.findElementByTestID(
      'touchable_without_feedback_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_without_feedback_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can register feedback events', async () => {
    const component = await app.findElementByTestID(
      'touchable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_feedback_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Text components can be tappable', async () => {
    const component = await app.findElementByTestID('tappable_text');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('tappable_text');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can delay events', async () => {
    const component = await app.findElementByTestID(
      'touchable_delay_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_delay_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can enable a hit slop region', async () => {
    const component = await app.findElementByTestID(
      'touchable_hit_slop_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_hit_slop_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can be defined in a set using accessibilityPosInSet and accessibilitySetSize', async () => {
    const component = await app.findElementByTestID('touchable_set');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_set');
    expect(dump).toMatchSnapshot();
  });
});
