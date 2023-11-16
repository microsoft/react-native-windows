/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

//import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Button');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Button Tests', () => {
  // Disable until tests pass CI
  // test('Buttons have default styling', async () => {
  //   const component = await app.findElementByTestID('button_default_styling');
  //   await component.waitForDisplayed({timeout: 5000});
  //   const dump = await dumpVisualTree('button_default_styling');
  //   expect(dump).toMatchSnapshot();
  // });
  // test('Buttons can have custom colors', async () => {
  //   const component = await app.findElementByTestID('cancel_button');
  //   await component.waitForDisplayed({timeout: 5000});
  //   const dump = await dumpVisualTree('cancel_button');
  //   expect(dump).toMatchSnapshot();
  // });
  // test('Buttons can be disabled', async () => {
  //   const component = await app.findElementByTestID('disabled_button');
  //   await component.waitForDisplayed({timeout: 5000});
  //   const dump = await dumpVisualTree('disabled_button');
  //   expect(dump).toMatchSnapshot();
  // });
  // test('Buttons can have accessibility labels', async () => {
  //   const component = await app.findElementByTestID(
  //     'accessibilityLabel_button',
  //   );
  //   await component.waitForDisplayed({timeout: 5000});
  //   const dump = await dumpVisualTree('accessibilityLabel_button');
  //   expect(dump).toMatchSnapshot();
  // });
  // test('Buttons can have accessibility states', async () => {
  //   const component = await app.findElementByTestID(
  //     'accessibilityState_button',
  //   );
  //   await component.waitForDisplayed({timeout: 5000});
  //   const dump = await dumpVisualTree('accessibilityState_button');
  //   expect(dump).toMatchSnapshot();
  // });
  // test('Buttons can have accessibility props', async () => {
  //   const component = await app.findElementByTestID('accessibility_props');
  //   await component.waitForDisplayed({timeout: 5000});
  //   const dump = await dumpVisualTree('accessibility_props');
  //   expect(dump).toMatchSnapshot();
  // });
});
