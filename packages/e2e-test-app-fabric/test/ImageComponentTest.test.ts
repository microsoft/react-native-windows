/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { dumpVisualTree } from '@react-native-windows/automation-commands';
import { goToComponentExample } from './RNTesterNavigation';
import { app } from '@react-native-windows/automation';
import { verifyNoErrorLogs } from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Image');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

// TODO: Re-enable once element accessibility issues are resolved
describe.skip('Image Tests', () => {
  test('An Image component can render a blob image', async () => {
    const component = await app.findElementByTestID('image-blob');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-blob');
    expect(dump).toMatchSnapshot();
  });
  test('An Image component can have a network callback', async () => {
    const component = await app.findElementByTestID('image-network-callback');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-network-callback');
    expect(dump).toMatchSnapshot();
  });
  test('A network Image example', async () => {
    const component = await app.findElementByTestID('image-network');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-network');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can be rendered at a different size', async () => {
    const component = await app.findElementByTestID('image-size');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-size');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have multiple sources', async () => {
    const component = await app.findElementByTestID('image-multiple-sources');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-multiple-sources');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have a loading indicator visual', async () => {
    const component = await app.findElementByTestID('image-loading-indicator');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-loading-indicator');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have onLayout behavior', async () => {
    const component = await app.findElementByTestID('image-onlayout');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-onlayout');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can load an image through its source prop', async () => {
    const component = await app.findElementByTestID('image-plain-source');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-plain-source');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can load an image through its src prop', async () => {
    const component = await app.findElementByTestID('image-plain-src');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-plain-src');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have multiple sources through the srcSet prop', async () => {
    const component = await app.findElementByTestID('image-srcset');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-srcset');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can load a static image', async () => {
    const component = await app.findElementByTestID('image-static');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-static');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can borders', async () => {
    const component = await app.findElementByTestID('image-border');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-border');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have border colors', async () => {
    const component = await app.findElementByTestID('image-border-colors');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-border-colors');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can a border radius', async () => {
    const component = await app.findElementByTestID('image-border-radius');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-border-radius');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have a background color', async () => {
    const component = await app.findElementByTestID('background-color-example');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('background-color-example');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have custom opacity', async () => {
    const component = await app.findElementByTestID('image-opacity');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-opacity');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can be nested inside of a component', async () => {
    const component = await app.findElementByTestID('image-nested');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-nested');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have a tint color', async () => {
    const component = await app.findElementByTestID('image-tint-color');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-tint-color');
    expect(dump).toMatchSnapshot();
  });
  test('An Image customized how it is rendered within the frame using the objectFit styles', async () => {
    const component = await app.findElementByTestID('image-object-fit');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-object-fit');
    expect(dump).toMatchSnapshot();
  });
  test('Images have multiple resize modes', async () => {
    const component = await app.findElementByTestID('image-resize-mode');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-resize-mode');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have a custom blur radius', async () => {
    const component = await app.findElementByTestID('image-blur-radius');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-blur-radius');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can be added to the accessibility tree', async () => {
    const component = await app.findElementByTestID('image-accessible');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-accessible');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have an accessibilityLabel', async () => {
    const component = await app.findElementByTestID('image-accessibilitylabel');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-accessibilitylabel');
    expect(dump).toMatchSnapshot();
  });
  test('An Image can have a can have alt text', async () => {
    const component = await app.findElementByTestID('image-alt');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-alt');
    expect(dump).toMatchSnapshot();
  });
  test('Images can be defined as a set using accessibilityPosInSet and accessibilitySetSize', async () => {
    const component = await app.findElementByTestID('image-set');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-set');
    expect(dump).toMatchSnapshot();
  });
  test('An Image supports accessibility properties', async () => {
    const component = await app.findElementByTestID(
      'image-accessibility-properties',
    );
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-accessibility-properties');
    expect(dump).toMatchSnapshot();
  });
  test('An Image supports style properties', async () => {
    const component = await app.findElementByTestID('image-style-properties');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-style-properties');
    expect(dump).toMatchSnapshot();
  });
  test('An Image supports interaction properties', async () => {
    const component = await app.findElementByTestID(
      'image-interaction-properties',
    );
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-interaction-properties');
    expect(dump).toMatchSnapshot();
  });
  test('An Image supports abortPrefetch and resolveAssetSource', async () => {
    const component = await app.findElementByTestID('image-asset-source');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-asset-source');
    expect(dump).toMatchSnapshot();
  });
  test('An Image supports accessibility actions', async () => {
    const component = await app.findElementByTestID(
      'image-accessibility-actions',
    );
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('image-accessibility-actions');
    expect(dump).toMatchSnapshot();
  });
});
