/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for View component.
 *
 * @format
 */

import * as React from 'react';
import {View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class ViewPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'View';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-view';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return <View testID={this.testId} style={styles.container} {...props} />;
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'nested-views-50',
        description: 'Render 50 nested View components',
        run: () => this.measureNestedViews(50),
      },
      {
        name: 'nested-views-100',
        description: 'Render 100 nested View components',
        run: () => this.measureNestedViews(100),
      },
      {
        name: 'with-shadow',
        description: 'View with shadow styles',
        run: () => this.measureWithShadow(),
      },
      {
        name: 'with-border-radius',
        description: 'View with complex border radius',
        run: () => this.measureWithBorderRadius(),
      },
    ];
  }

  private async measureNestedViews(count: number): Promise<PerfMetrics> {
    const NestedViews = () => (
      <View testID={this.testId} style={styles.container}>
        {Array.from({length: count}, (_, i) => (
          <View key={i} style={styles.nested} />
        ))}
      </View>
    );

    return measurePerf(<NestedViews />, {
      name: `${this.componentName} nested-${count}`,
      runs: 15, // More runs for stable median on heavy scenarios
    });
  }

  private async measureWithShadow(): Promise<PerfMetrics> {
    return measurePerf(
      <View testID={this.testId} style={[styles.container, styles.shadow]} />,
      {
        name: `${this.componentName} shadow`,
        runs: 10,
      },
    );
  }

  private async measureWithBorderRadius(): Promise<PerfMetrics> {
    return measurePerf(
      <View
        testID={this.testId}
        style={[styles.container, styles.borderRadius]}
      />,
      {
        name: `${this.componentName} border-radius`,
        runs: 10,
      },
    );
  }
}

// ─── TEST SUITE ───

const viewPerfTest = new ViewPerfTest();

describe('View Performance', () => {
  // Standard scenarios
  test('mount time', async () => {
    const perf = await viewPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await viewPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await viewPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  // Custom scenarios
  describe('View-Specific Scenarios', () => {
    test('nested-views-50', async () => {
      const scenario = viewPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({maxDurationIncrease: 15, minAbsoluteDelta: 5});
    });

    test('nested-views-100', async () => {
      const scenario = viewPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({maxDurationIncrease: 15, minAbsoluteDelta: 5});
    });

    test('with-shadow', async () => {
      const scenario = viewPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-border-radius', async () => {
      const scenario = viewPerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });
  });
});

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: 'white',
  },
  nested: {
    height: 10,
    backgroundColor: 'lightgray',
    margin: 1,
  },
  shadow: {
    shadowColor: '#000',
    shadowOffset: {width: 0, height: 2},
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
    elevation: 5,
  },
  borderRadius: {
    borderRadius: 10,
    borderWidth: 2,
    borderColor: 'blue',
  },
});
