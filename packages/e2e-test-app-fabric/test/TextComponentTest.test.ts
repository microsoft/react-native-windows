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
  await goToComponentExample('Text');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Text Tests', () => {
  test('Text can wrap', async () => {
    const component = await app.findElementByTestID('text-wrap');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-wrap');
    expect(dump).toMatchSnapshot();
  });
  test('Padding can be added to Text', async () => {
    const component = await app.findElementByTestID('text-padding');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-padding');
    expect(dump).toMatchSnapshot();
  });
  // Disabling until test passes.
  /*test('Text can have a font family', async () => {
    const component = await app.findElementByTestID('text-fontFamily');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-fontFamily');
    expect(dump).toMatchSnapshot();
  });*/
  test('Text can have a size', async () => {
    const component = await app.findElementByTestID('text-size');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-size');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have a color', async () => {
    const component = await app.findElementByTestID('text-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-color');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have decoration lines: Underline', async () => {
    const component = await app.findElementByTestID(
      'text-decoration-underline',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-decoration-underline');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have decoration lines: Solid Line Through', async () => {
    const component = await app.findElementByTestID(
      'text-decoration-solid-linethru',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-decoration-solid-linethru');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have an outer color', async () => {
    const component = await app.findElementByTestID('text-outer-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-outer-color');
    expect(dump).toMatchSnapshot();
  });
  test('Text can be restricted to one line', async () => {
    const component = await app.findElementByTestID('text-one-line');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-one-line');
    expect(dump).toMatchSnapshot();
  });
  test('Text can be selectable', async () => {
    const component = await app.findElementByTestID('text-selectable');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-selectable');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have a customized selection color', async () => {
    const component = await app.findElementByTestID('text-selection-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-selection-color');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have shadows', async () => {
    const component = await app.findElementByTestID('text-shadow');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-shadow');
    expect(dump).toMatchSnapshot();
  });
});
