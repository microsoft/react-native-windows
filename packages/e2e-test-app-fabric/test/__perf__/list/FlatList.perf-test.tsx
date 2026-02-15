/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for FlatList component.
 *
 * @format
 */

import * as React from 'react';
import {FlatList, Text, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

function generateItems(count: number): Array<{id: string; title: string}> {
  return Array.from({length: count}, (_, i) => ({
    id: String(i),
    title: `Item ${i}`,
  }));
}

const DATA_10 = generateItems(10);
const DATA_100 = generateItems(100);
const DATA_500 = generateItems(500);

const renderItem = ({item}: {item: {id: string; title: string}}) => (
  <View style={styles.item}>
    <Text>{item.title}</Text>
  </View>
);

const keyExtractor = (item: {id: string}) => item.id;

class FlatListPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'FlatList';
  readonly category = 'list' as const;
  readonly testId = 'perf-test-flat-list';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <FlatList
        testID={this.testId}
        data={DATA_10}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        style={styles.default}
        {...props}
      />
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'with-10-items',
        description: 'FlatList rendering 10 items',
        run: () => this.measureWithItems(DATA_10),
      },
      {
        name: 'with-100-items',
        description: 'FlatList rendering 100 items',
        run: () => this.measureWithItems(DATA_100),
      },
      {
        name: 'with-500-items',
        description: 'FlatList rendering 500 items',
        run: () => this.measureWithItems(DATA_500),
      },
      {
        name: 'horizontal',
        description: 'FlatList in horizontal layout mode',
        run: () => this.measureHorizontal(),
      },
      {
        name: 'with-separator',
        description: 'FlatList with ItemSeparatorComponent',
        run: () => this.measureWithSeparator(),
      },
      {
        name: 'with-header-footer',
        description: 'FlatList with ListHeaderComponent and ListFooterComponent',
        run: () => this.measureWithHeaderFooter(),
      },
      {
        name: 'with-empty-list',
        description: 'FlatList with empty data and ListEmptyComponent',
        run: () => this.measureWithEmptyList(),
      },
      {
        name: 'with-get-item-layout',
        description: 'FlatList with getItemLayout for optimized scrolling',
        run: () => this.measureWithGetItemLayout(),
      },
      {
        name: 'inverted',
        description: 'FlatList in inverted mode',
        run: () => this.measureInverted(),
      },
      {
        name: 'with-num-columns',
        description: 'FlatList rendered as a grid with numColumns=3',
        run: () => this.measureWithNumColumns(),
      },
    ];
  }

  private async measureWithItems(
    data: Array<{id: string; title: string}>,
  ): Promise<PerfMetrics> {
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={data}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        style={styles.default}
      />,
      {name: `${this.componentName} with-${data.length}-items`, runs: 10},
    );
  }

  private async measureHorizontal(): Promise<PerfMetrics> {
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={DATA_100}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        horizontal={true}
        style={styles.default}
      />,
      {name: `${this.componentName} horizontal`, runs: 10},
    );
  }

  private async measureWithSeparator(): Promise<PerfMetrics> {
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={DATA_100}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        ItemSeparatorComponent={() => <View style={styles.separator} />}
        style={styles.default}
      />,
      {name: `${this.componentName} with-separator`, runs: 10},
    );
  }

  private async measureWithHeaderFooter(): Promise<PerfMetrics> {
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={DATA_100}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        ListHeaderComponent={
          <View style={styles.header}>
            <Text>Header</Text>
          </View>
        }
        ListFooterComponent={
          <View style={styles.footer}>
            <Text>Footer</Text>
          </View>
        }
        style={styles.default}
      />,
      {name: `${this.componentName} with-header-footer`, runs: 10},
    );
  }

  private async measureWithEmptyList(): Promise<PerfMetrics> {
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={[]}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        ListEmptyComponent={
          <View style={styles.empty}>
            <Text>No items to display</Text>
          </View>
        }
        style={styles.default}
      />,
      {name: `${this.componentName} with-empty-list`, runs: 10},
    );
  }

  private async measureWithGetItemLayout(): Promise<PerfMetrics> {
    const ITEM_HEIGHT = 44;
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={DATA_100}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        getItemLayout={(_data, index) => ({
          length: ITEM_HEIGHT,
          offset: ITEM_HEIGHT * index,
          index,
        })}
        style={styles.default}
      />,
      {name: `${this.componentName} with-get-item-layout`, runs: 10},
    );
  }

  private async measureInverted(): Promise<PerfMetrics> {
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={DATA_100}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        inverted={true}
        style={styles.default}
      />,
      {name: `${this.componentName} inverted`, runs: 10},
    );
  }

  private async measureWithNumColumns(): Promise<PerfMetrics> {
    return measurePerf(
      <FlatList
        testID={this.testId}
        data={DATA_100}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
        numColumns={3}
        style={styles.default}
      />,
      {name: `${this.componentName} with-num-columns`, runs: 10},
    );
  }
}

const flatListPerfTest = new FlatListPerfTest();

describe('FlatList Performance', () => {
  test('mount time', async () => {
    const perf = await flatListPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await flatListPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await flatListPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('FlatList-Specific Scenarios', () => {
    const scenarios = flatListPerfTest.getCustomScenarios();

    test('with-10-items', async () => {
      const perf = await scenarios[0].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-100-items', async () => {
      const perf = await scenarios[1].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-500-items', async () => {
      const perf = await scenarios[2].run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 15,
        minAbsoluteDelta: 10,
      });
    });

    test('horizontal', async () => {
      const perf = await scenarios[3].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-separator', async () => {
      const perf = await scenarios[4].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-header-footer', async () => {
      const perf = await scenarios[5].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-empty-list', async () => {
      const perf = await scenarios[6].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-get-item-layout', async () => {
      const perf = await scenarios[7].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('inverted', async () => {
      const perf = await scenarios[8].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-num-columns', async () => {
      const perf = await scenarios[9].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });
  });
});

const styles = StyleSheet.create({
  default: {
    height: 300,
  },
  item: {
    padding: 10,
    borderBottomWidth: 1,
    borderBottomColor: '#ccc',
  },
  separator: {
    height: 1,
    backgroundColor: '#ddd',
  },
  header: {
    padding: 10,
    backgroundColor: '#f0f0f0',
  },
  footer: {
    padding: 10,
    backgroundColor: '#f0f0f0',
  },
  empty: {
    padding: 20,
    alignItems: 'center',
  },
});
