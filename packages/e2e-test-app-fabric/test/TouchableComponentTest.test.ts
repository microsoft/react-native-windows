/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('Touchable* and onPress');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Touchable Tests', () => {
  test('Touchables can contain a Text component', async () => {
    const dump = await dumpVisualTree('touchable_highlight_text_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can contain an Image component', async () => {
    const dump = await dumpVisualTree('touchable_highlight_image_button');
    expect(dump).toMatchSnapshot();
  });
  test('TouchableWithoutFeedback components should not give visual feedback', async () => {
    const dump = await dumpVisualTree('touchable_without_feedback_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can register feedback events', async () => {
    const dump = await dumpVisualTree('touchable_feedback_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Text components can be tappable', async () => {
    const dump = await dumpVisualTree('tappable_text');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can delay events', async () => {
    const dump = await dumpVisualTree('touchable_delay_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can enable a hit slop region', async () => {
    const dump = await dumpVisualTree('touchable_hit_slop_button');
    expect(dump).toMatchSnapshot();
  });
});
