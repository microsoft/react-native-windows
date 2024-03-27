/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {app} from '@react-native-windows/automation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
});

beforeEach(async () => {
  await goToComponentExample('FlatList'); // multiple FlatList pages so we need to reset between each test.
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

const searchBox = async (input: string) => {
  const searchBox = await app.findElementByTestID('search_bar_flat_list');
  await app.waitUntil(
    async () => {
      await searchBox.setValue(input);
      return (await searchBox.getText()) === input;
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );
};

const goToFlatListExample = async (input: string) => {
  const testPage = await app.findElementByTestID(input);
  await testPage.waitForDisplayed({timeout: 5000});
  await testPage.click();
};

describe('FlatList Tests', () => {
  test('A FlatList can display a list of components', async () => {
    await goToFlatListExample('Basic');
    const component = await app.findElementByTestID('flatlist-basic');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flatlist-basic');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can be filtered by a key word', async () => {
    await goToFlatListExample('Basic');
    await searchBox('555');
    const component = await app.findElementByTestID('flatlist-basic');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flatlist-basic');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList has an onStartReached event', async () => {
    await goToFlatListExample('onStartReached');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList has an onEndReached event', async () => {
    await goToFlatListExample('onEndReached');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can have a content inset', async () => {
    await goToFlatListExample('Content Inset');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can be inverted', async () => {
    await goToFlatListExample('Inverted');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can set onViewableItemsChanged', async () => {
    await goToFlatListExample('onViewableItemsChanged');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can have separators', async () => {
    await goToFlatListExample('FlatList with Separators');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can have multiple columns', async () => {
    await goToFlatListExample('MultiColumn');
    const component = await app.findElementByTestID('flatlist-multicolumn');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flatlist-multicolumn');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can have sticky headers', async () => {
    await goToFlatListExample('Sticky Headers');
    const component = await app.findElementByTestID('flatlist-sticky');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flatlist-sticky');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can nest other Flatlists', async () => {
    await goToFlatListExample('Nested');
    const component = await app.findElementByTestID('flatlist-nested');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flatlist-nested');
    expect(dump).toMatchSnapshot();
  });
});
