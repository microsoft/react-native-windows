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

type VisualNode = Awaited<ReturnType<typeof dumpVisualTree>>['Visual Tree'];

const parseVector = (value: string | undefined) =>
  value?.split(',').map(Number) ?? [];

const normalizeFlatListRows = (root: VisualNode) => {
  const normalizers: Array<() => void> = [];

  const visit = (node: VisualNode) => {
    const children = node.__Children;
    if (!children) {
      return;
    }

    const [parentWidth] = parseVector(node.Size);
    const rows = children
      .map((child, index) => {
        const [x, y] = parseVector(child.Offset);
        const [width, height] = parseVector(child.Size);
        return {child, height, index, width, x, y};
      })
      .filter(
        row => row.x === 0 && row.width === parentWidth && row.height > 0,
      );
    const orderedRows = [...rows].sort((left, right) => left.y - right.y);
    const rowsDoNotOverlap = orderedRows.every(
      (row, index) =>
        index === 0 ||
        orderedRows[index - 1].y + orderedRows[index - 1].height <= row.y,
    );

    if (rows.length === 3 && rowsDoNotOverlap) {
      normalizers.push(() => {
        rows.forEach((row, index) => {
          children[row.index] = orderedRows[index].child;
        });
      });
    }

    children.forEach(visit);
  };

  visit(root);
  if (normalizers.length === 1) {
    normalizers[0]();
  }
  return normalizers.length;
};

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
  const searchBox = await app.findElementByTestID('example_search');
  await app.waitUntil(
    async () => {
      // Clear before each attempt: WinAppDriver's setValue can fall back to
      // synthesized keystrokes for custom RN TextInputs, which append rather
      // than replace. Without the clear, a retry produces concatenated text
      // and the comparison never converges.
      await searchBox.clearValue();
      await searchBox.setValue(input);
      return (await searchBox.getText()) === input;
    },
    {
      interval: 500,
      timeout: 10000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );
};

const searchBoxBasic = async (input: string) => {
  const searchBox = await app.findElementByTestID('search_bar_flat_list');
  await app.waitUntil(
    async () => {
      // See comment in searchBox above for the clearValue rationale.
      await searchBox.clearValue();
      await searchBox.setValue(input);
      return (await searchBox.getText()) === input;
    },
    {
      interval: 500,
      timeout: 10000,
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
  test('A FlatList can be filtered by a key word', async () => {
    await searchBox('Basic');
    await goToFlatListExample('Basic');
    await searchBoxBasic('555');
    const component = await app.findElementByTestID('flatlist-basic');
    await component.waitForDisplayed({timeout: 5000});
    let dump = await dumpVisualTree('flatlist-basic');
    await app.waitUntil(
      async () => {
        dump = await dumpVisualTree('flatlist-basic');
        const automationTree = JSON.stringify(dump['Automation Tree']);
        return (
          automationTree.includes('Item 555 -') &&
          !automationTree.includes('LIST HEADER')
        );
      },
      {
        interval: 250,
        timeout: 20000,
        timeoutMsg: 'Filtered FlatList did not finish rendering.',
      },
    );
    expect(normalizeFlatListRows(dump['Visual Tree'])).toBe(1);
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList has an onStartReached event', async () => {
    await searchBox('onStartReached');
    await goToFlatListExample('onStartReached');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList has an onEndReached event', async () => {
    await searchBox('onEndReached');
    await goToFlatListExample('onEndReached');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can have a content inset', async () => {
    await searchBox('Content');
    await goToFlatListExample('Content Inset');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can be inverted', async () => {
    await searchBox('Inverted');
    await goToFlatListExample('Inverted');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can have separators', async () => {
    await searchBox('separators');
    await goToFlatListExample('FlatList with Separators');
    const component = await app.findElementByTestID('flat_list');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flat_list');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can have sticky headers', async () => {
    await searchBox('Sticky');
    await goToFlatListExample('Sticky Headers');
    const component = await app.findElementByTestID('flatlist-sticky');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flatlist-sticky');
    expect(dump).toMatchSnapshot();
  });
  test('A FlatList can nest other Flatlists', async () => {
    await searchBox('Nested');
    await goToFlatListExample('Nested');
    const component = await app.findElementByTestID('flatlist-nested');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flatlist-nested');
    expect(dump).toMatchSnapshot();
  });
});
