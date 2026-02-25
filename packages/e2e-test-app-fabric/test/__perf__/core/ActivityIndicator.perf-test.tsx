/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for ActivityIndicator component.
 *
 * @format
 */

import * as React from 'react';
import {ActivityIndicator, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class ActivityIndicatorPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'ActivityIndicator';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-activity-indicator';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return <ActivityIndicator testID={this.testId} {...props} />;
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'size-large',
        description: 'ActivityIndicator with size="large"',
        run: () => this.measureSizeLarge(),
      },
      {
        name: 'size-small',
        description: 'ActivityIndicator with size="small"',
        run: () => this.measureSizeSmall(),
      },
      {
        name: 'with-color',
        description: 'ActivityIndicator with custom color',
        run: () => this.measureWithColor(),
      },
      {
        name: 'not-animating',
        description: 'ActivityIndicator with animating={false}',
        run: () => this.measureNotAnimating(),
      },
      {
        name: 'with-accessibility',
        description: 'ActivityIndicator with accessibility props',
        run: () => this.measureWithAccessibility(),
      },
      {
        name: 'multiple-indicators-10',
        description: 'Render 10 sibling ActivityIndicators',
        run: () => this.measureMultiple(10),
      },
      {
        name: 'multiple-indicators-50',
        description: 'Render 50 sibling ActivityIndicators',
        run: () => this.measureMultiple(50),
      },
      {
        name: 'multiple-indicators-100',
        description: 'Render 100 sibling ActivityIndicators (stress gate)',
        run: () => this.measureMultiple(100),
      },
    ];
  }

  private async measureSizeLarge(): Promise<PerfMetrics> {
    return measurePerf(
      <ActivityIndicator testID={this.testId} size="large" />,
      {
        name: `${this.componentName} size-large`,
        runs: 10,
      },
    );
  }

  private async measureSizeSmall(): Promise<PerfMetrics> {
    return measurePerf(
      <ActivityIndicator testID={this.testId} size="small" />,
      {
        name: `${this.componentName} size-small`,
        runs: 10,
      },
    );
  }

  private async measureWithColor(): Promise<PerfMetrics> {
    return measurePerf(
      <ActivityIndicator testID={this.testId} size="large" color="#841584" />,
      {
        name: `${this.componentName} with-color`,
        runs: 10,
      },
    );
  }

  private async measureNotAnimating(): Promise<PerfMetrics> {
    return measurePerf(
      <ActivityIndicator testID={this.testId} animating={false} />,
      {
        name: `${this.componentName} not-animating`,
        runs: 10,
      },
    );
  }

  private async measureWithAccessibility(): Promise<PerfMetrics> {
    return measurePerf(
      <ActivityIndicator
        testID={this.testId}
        size="large"
        accessibilityLabel="Loading content"
        accessibilityHint="Please wait"
        accessibilityRole="progressbar"
      />,
      {
        name: `${this.componentName} with-accessibility`,
        runs: 10,
      },
    );
  }

  private async measureMultiple(count: number): Promise<PerfMetrics> {
    const IndicatorList = () => (
      <View style={styles.container}>
        {Array.from({length: count}, (_, i) => (
          <ActivityIndicator
            key={i}
            size={i % 2 === 0 ? 'small' : 'large'}
            color={i % 3 === 0 ? '#841584' : undefined}
          />
        ))}
      </View>
    );

    return measurePerf(<IndicatorList />, {
      name: `${this.componentName} multiple-${count}`,
      runs: 15,
    });
  }
}

const activityIndicatorPerfTest = new ActivityIndicatorPerfTest();

describe('ActivityIndicator Performance', () => {
  test('mount time', async () => {
    const perf = await activityIndicatorPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await activityIndicatorPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await activityIndicatorPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('ActivityIndicator-Specific Scenarios', () => {
    test('size-large', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('size-small', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-color', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('not-animating', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-accessibility', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-indicators-10', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[5];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('multiple-indicators-50', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[6];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 15,
        minAbsoluteDelta: 5,
      });
    });

    test('multiple-indicators-100', async () => {
      const scenario = activityIndicatorPerfTest.getCustomScenarios()[7];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 10,
        minAbsoluteDelta: 10,
        mode: 'gate',
      });
    });
  });
});

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
});
