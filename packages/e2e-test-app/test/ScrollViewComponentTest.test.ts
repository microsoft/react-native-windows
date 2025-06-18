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
  await goToComponentExample('ScrollView');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('ScrollViewTests', () => {
  test('ScrollViews can scroll an item list vertically', async () => {
    const dump = await dumpVisualTree('scroll_vertical');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollView has scrollTo method, scroll to top button', async () => {
    const dump = await dumpVisualTree('scroll_to_top_button');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollView has scrollTo method, scroll to bottom button', async () => {
    const dump = await dumpVisualTree('scroll_to_bottom_button');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollViews has flash scroll indicators', async () => {
    const dump = await dumpVisualTree('flash_scroll_indicators_button');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollViews can scroll an item list horizontally', async () => {
    const dump = await dumpVisualTree('scroll_horizontal');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollView has scrollTo method, scroll to start button', async () => {
    const dump = await dumpVisualTree('scroll_to_start_button');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollView has scrollTo method, scroll to end button', async () => {
    const dump = await dumpVisualTree('scroll_to_end_button');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollViews can have sticky headers', async () => {
    const dump = await dumpVisualTree('scroll_sticky_header');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollViews can have multiple sticky headers', async () => {
    const dump = await dumpVisualTree('scroll_multiple_sticky_headers');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollViews can have pressable sticky headers', async () => {
    const dump = await dumpVisualTree('scroll_pressable_sticky_header');
    expect(dump).toMatchSnapshot();
  });
  test('ScrollViews can use snapToOffsets for snap-to-points behavior', async () => {
    // Find and interact with snap to offsets button to enable snapping
    const snapButton = await $(
      "~snapToOffsets === [] ? 'setSnapToOffsets' : 'reset snapToOffsets'",
    );
    await snapButton.waitForDisplayed({timeout: 10000});
    await snapButton.click();

    // Get the visual tree of the ScrollView after enabling snap points
    const dump = await dumpVisualTree('scroll_snap_to_options');
    expect(dump).toMatchSnapshot();
  });
});
