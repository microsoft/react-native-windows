/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { app } from '@react-native-windows/automation';
import { dumpVisualTree } from '@react-native-windows/automation-commands';
import { goToComponentExample } from './RNTesterNavigation';
import { verifyNoErrorLogs } from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('LegacyTextHitTestTest');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

// TODO: Re-enable once https://github.com/microsoft/react-native-windows/issues/15497 is fixed
describe.skip('LegacyTextHitTestTest', () => {
  beforeEach(async () => {
    await clearState();
  });

  test('VirtualTextPressable', async () => {
    await clickAt('virtual-text', { pctX: 0.85 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('TextPressableWithVirtualText', async () => {
    await clickAt('nested-text', { pctX: 0.9 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('MultilineTextPressable', async () => {
    await clickAt('multiline-text', { pctX: 0.15, pctY: 0.75 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('MultilineRTLTextPressable', async () => {
    await clickAt('multiline-rtl-text', { pctX: 0.85, pctY: 0.25 });
    await clickAt('multiline-rtl-text', { pctX: 0.1, pctY: 0.75 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('RTLTextPressable', async () => {
    await clickAt('rtl-text', { pctX: 0.85 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('RTLTextInRTLFlowPressable', async () => {
    await clickAt('rtl-rtl-flow-text', { pctX: 0.8 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('LTRTextInRTLFlowPressable', async () => {
    await clickAt('ltr-rtl-flow-text', { pctX: 0.9 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('BidirectionalTextPressable', async () => {
    await clickAt('bidirectional-text', { pctX: 0.8 });
    await clickAt('bidirectional-text', { pctX: 0.95 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('BidirectionalTextSeparateRunsPressable', async () => {
    await clickAt('separate-bidirectional-text', { pctX: 0.8 });
    await clickAt('separate-bidirectional-text', { pctX: 0.95 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('BidirectionalTextSeparateRunsEdgeCasePressable', async () => {
    await clickAt('separate-bidirectional-text', { pctX: 0.62 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('InsertedVirtualTextPressable', async () => {
    await clickAt('inserted-text', { pctX: 0.85 });
    await clickAt('inserted-text', { pctX: 0.9 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('ToggleVirtualTextPressable', async () => {
    await clickAt('toggled-text', { pctX: 0.9 });
    await clickAt('toggled-text', { pctX: 0.9 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('WrappedLTRInRTLFlowEdgeCaseNotPressable', async () => {
    await clickAt('wrapped-ltr-rtl-flow-text', { pctX: 0.85, pctY: 0.25 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('MultilineRTLTextEdgeCaseNotPressable', async () => {
    await clickAt('multiline-rtl-text', { pctX: 0.765, pctY: 0.25 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('BidirectionalTextPressableEdgeCaseNotPressable', async () => {
    await clickAt('bidirectional-text', { pctX: 0.61 });
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });
});

async function clearState() {
  const clearButton = await app.findElementByTestID('clear-state-button');
  await clearButton.click();
}

async function clickAt(
  name: string,
  { pctX, pctY = 0.5 }: { pctX: number; pctY?: number },
) {
  const target = await app.findElementByTestID(name);
  const { width, height } = await target.getSize();

  // The webdriverio package computes the offsets from the center point of the
  // target. Additionally, the package uses raw pixels. To make this test more
  // robust to different screen resolutions and scale settings, the test cases
  // are declared using relatively percentages for where in the hit target to
  // click, and subtracts the center point so the declared offsets are absolute
  // relative to the origin of the target (i.e., top-left corner).
  await target.click({
    x: Math.round(pctX * width - width / 2),
    y: Math.round(pctY * height - height / 2),
  });
}
