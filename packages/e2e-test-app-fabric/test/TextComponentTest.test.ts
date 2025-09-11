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
  test('Text can have borders', async () => {
    const component = await app.findElementByTestID('text-border');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-border');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have advanced borders', async () => {
    const component = await app.findElementByTestID('advanced-borders');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('advanced-borders');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have inline views/images', async () => {
    const component = await app.findElementByTestID('text-view');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-view');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have nested views', async () => {
    const component = await app.findElementByTestID('text-nested-view');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-nested-view');
    expect(dump).toMatchSnapshot();
  });
  test('Texts can clip inline View/Images', async () => {
    const component = await app.findElementByTestID('text-view-images-clipped');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-view-images-clipped');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, default a', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-default-a',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-default-a');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, default b', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-default-b',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-default-b');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, case 0 a', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-0-a',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-0-a');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, case 0 b', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-0-b',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-0-b');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, case 1 a', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-1-a',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-1-a');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, case 1 b', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-1-b',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-1-b');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, case 2 a', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-2-a',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-2-a');
    expect(dump).toMatchSnapshot();
  });
  test('Text can adjust its fontsize according to its limitations, case 2 b', async () => {
    const component = await app.findElementByTestID(
      'text-adjustfontsizetofit-2-b',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-adjustfontsizetofit-2-b');
    expect(dump).toMatchSnapshot();
  });

  /* For some reason WebDriver can't find this view even though accessible={true}
  test('Texts can align inline View/Images', async () => {
    const component = await app.findElementByTestID(
      'view-test-inline-text-alignment',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('view-test-inline-text-alignment');
    expect(dump).toMatchSnapshot();
  });
  */
});
