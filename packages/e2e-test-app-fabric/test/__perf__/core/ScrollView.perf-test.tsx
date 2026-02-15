/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for ScrollView component.
 *
 * @format
 */

import * as React from 'react';
import {ScrollView, View, Text, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class ScrollViewPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'ScrollView';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-scroll-view';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <ScrollView
        testID={this.testId}
        style={styles.default}
        {...props}>
        <Text>Sample Content</Text>
      </ScrollView>
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'with-children-20',
        description: 'ScrollView with 20 child items',
        run: () => this.measureWithChildren(20),
      },
      {
        name: 'with-children-100',
        description: 'ScrollView with 100 child items',
        run: () => this.measureWithChildren(100),
      },
      {
        name: 'horizontal',
        description: 'Horizontal ScrollView with children',
        run: () => this.measureHorizontal(),
      },
      {
        name: 'with-sticky-headers',
        description: 'ScrollView with stickyHeaderIndices',
        run: () => this.measureWithStickyHeaders(),
      },
      {
        name: 'with-scroll-indicators',
        description: 'ScrollView with indicator style and bar visibility',
        run: () => this.measureWithScrollIndicators(),
      },
      {
        name: 'nested-scroll-views',
        description: 'ScrollView nested inside another ScrollView',
        run: () => this.measureNestedScrollViews(),
      },
      {
        name: 'with-content-container-style',
        description: 'ScrollView with contentContainerStyle',
        run: () => this.measureWithContentContainerStyle(),
      },
    ];
  }

  private renderItems(count: number): React.ReactElement[] {
    return Array.from({length: count}, (_, i) => (
      <View key={i} style={styles.item}>
        <Text>{`Item ${i}`}</Text>
      </View>
    ));
  }

  private async measureWithChildren(count: number): Promise<PerfMetrics> {
    const ScrollList = () => (
      <ScrollView testID={this.testId} style={styles.default}>
        {this.renderItems(count)}
      </ScrollView>
    );

    return measurePerf(<ScrollList />, {
      name: `${this.componentName} children-${count}`,
      runs: 15,
    });
  }

  private async measureHorizontal(): Promise<PerfMetrics> {
    const HorizontalScroll = () => (
      <ScrollView
        testID={this.testId}
        style={styles.default}
        horizontal={true}
        showsHorizontalScrollIndicator={true}>
        {Array.from({length: 20}, (_, i) => (
          <View key={i} style={styles.horizontalItem}>
            <Text>{`H${i}`}</Text>
          </View>
        ))}
      </ScrollView>
    );

    return measurePerf(<HorizontalScroll />, {
      name: `${this.componentName} horizontal`,
      runs: 10,
    });
  }

  private async measureWithStickyHeaders(): Promise<PerfMetrics> {
    const StickyScroll = () => (
      <ScrollView
        testID={this.testId}
        style={styles.default}
        stickyHeaderIndices={[0, 5, 10, 15]}>
        {Array.from({length: 20}, (_, i) => (
          <View
            key={i}
            style={[styles.item, i % 5 === 0 && styles.stickyHeader]}>
            <Text>{i % 5 === 0 ? `Section ${i / 5}` : `Item ${i}`}</Text>
          </View>
        ))}
      </ScrollView>
    );

    return measurePerf(<StickyScroll />, {
      name: `${this.componentName} sticky-headers`,
      runs: 10,
    });
  }

  private async measureWithScrollIndicators(): Promise<PerfMetrics> {
    return measurePerf(
      <ScrollView
        testID={this.testId}
        style={styles.default}
        showsVerticalScrollIndicator={true}
        showsHorizontalScrollIndicator={false}
        scrollIndicatorInsets={{top: 10, bottom: 10}}>
        {this.renderItems(20)}
      </ScrollView>,
      {
        name: `${this.componentName} scroll-indicators`,
        runs: 10,
      },
    );
  }

  private async measureNestedScrollViews(): Promise<PerfMetrics> {
    const NestedScroll = () => (
      <ScrollView testID={this.testId} style={styles.default}>
        <Text>Outer Header</Text>
        <ScrollView horizontal={true} style={styles.innerScroll}>
          {Array.from({length: 10}, (_, i) => (
            <View key={i} style={styles.horizontalItem}>
              <Text>{`Inner ${i}`}</Text>
            </View>
          ))}
        </ScrollView>
        {this.renderItems(10)}
      </ScrollView>
    );

    return measurePerf(<NestedScroll />, {
      name: `${this.componentName} nested`,
      runs: 10,
    });
  }

  private async measureWithContentContainerStyle(): Promise<PerfMetrics> {
    return measurePerf(
      <ScrollView
        testID={this.testId}
        style={styles.default}
        contentContainerStyle={styles.contentContainer}>
        {this.renderItems(20)}
      </ScrollView>,
      {
        name: `${this.componentName} content-container-style`,
        runs: 10,
      },
    );
  }
}

// ─── TEST SUITE ───

const scrollViewPerfTest = new ScrollViewPerfTest();

describe('ScrollView Performance', () => {
  test('mount time', async () => {
    const perf = await scrollViewPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await scrollViewPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await scrollViewPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('ScrollView-Specific Scenarios', () => {
    test('with-children-20', async () => {
      const scenario = scrollViewPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-children-100', async () => {
      const scenario = scrollViewPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({maxDurationIncrease: 15, minAbsoluteDelta: 5});
    });

    test('horizontal', async () => {
      const scenario = scrollViewPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-sticky-headers', async () => {
      const scenario = scrollViewPerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-scroll-indicators', async () => {
      const scenario = scrollViewPerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('nested-scroll-views', async () => {
      const scenario = scrollViewPerfTest.getCustomScenarios()[5];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-content-container-style', async () => {
      const scenario = scrollViewPerfTest.getCustomScenarios()[6];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });
  });
});

const styles = StyleSheet.create({
  default: {
    flex: 1,
    backgroundColor: 'white',
  },
  item: {
    height: 40,
    padding: 10,
    borderBottomWidth: 1,
    borderBottomColor: '#eee',
  },
  horizontalItem: {
    width: 80,
    height: 60,
    padding: 10,
    margin: 4,
    backgroundColor: '#f0f0f0',
    justifyContent: 'center',
    alignItems: 'center',
  },
  stickyHeader: {
    backgroundColor: '#ddd',
    fontWeight: 'bold',
  },
  innerScroll: {
    height: 80,
    marginVertical: 10,
  },
  contentContainer: {
    padding: 16,
    paddingBottom: 40,
  },
});
