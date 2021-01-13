/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import ImageTestPage from '../pages/ImageTestPage';
import { IMAGE_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(async () => {
  await HomePage.goToComponentExample(IMAGE_TESTPAGE);
});

describe('ImageWithoutBorderTest', async () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  it('ImageWithoutBorderTest', async () => {
    await ImageTestPage.waitForTreeDumpPassed(
      '#1. Dump comparison for image without border!'
    );
  });

  /* Test case #2: Click button once, update view and image with round border*/
  it('ImageWithBorderTest', async () => {
    await ImageTestPage.toggleImageBorder();
    await ImageTestPage.waitForTreeDumpPassed(
      '#2. Dump comparison for image with border!'
    );
  });

  /* Test case #3: Click button one more, remove border from view and image but tree sturcture is different from #1*/
  it('ImageWithoutBorderTestOneMoreClick', async () => {
    await ImageTestPage.toggleImageBorder();
    await ImageTestPage.waitForTreeDumpPassed(
      '#3. Second dump comparison for image without border!'
    );
  });

  it('ImageRTLTest', async () => {
    await ImageTestPage.toggleRTLMode();
    await ImageTestPage.waitForTreeDumpPassed('#4. Dump comparison for image RTL');
  });
});
