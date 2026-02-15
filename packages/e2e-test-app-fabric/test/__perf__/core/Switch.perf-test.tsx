/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for Switch component.
 *
 * @format
 */

import * as React from 'react';
import {Switch, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class SwitchPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'Switch';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-switch';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <Switch
        testID={this.testId}
        value={false}
        style={styles.default}
        {...props}
      />
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'value-true',
        description: 'Switch in the ON state',
        run: () => this.measureValueTrue(),
      },
      {
        name: 'disabled',
        description: 'Switch in disabled state',
        run: () => this.measureDisabled(),
      },
      {
        name: 'with-custom-colors',
        description: 'Switch with custom track and thumb colors',
        run: () => this.measureWithCustomColors(),
      },
      {
        name: 'with-on-value-change',
        description: 'Switch with onValueChange handler',
        run: () => this.measureWithOnValueChange(),
      },
      {
        name: 'with-accessibility',
        description: 'Switch with full accessibility props',
        run: () => this.measureWithAccessibility(),
      },
      {
        name: 'multiple-switches-10',
        description: 'Render 10 sibling Switches',
        run: () => this.measureMultipleSwitches(10),
      },
      {
        name: 'multiple-switches-50',
        description: 'Render 50 sibling Switches',
        run: () => this.measureMultipleSwitches(50),
      },
    ];
  }

  private async measureValueTrue(): Promise<PerfMetrics> {
    return measurePerf(
      <Switch testID={this.testId} value={true} />,
      {
        name: `${this.componentName} value-true`,
        runs: 10,
      },
    );
  }

  private async measureDisabled(): Promise<PerfMetrics> {
    return measurePerf(
      <Switch testID={this.testId} value={false} disabled={true} />,
      {
        name: `${this.componentName} disabled`,
        runs: 10,
      },
    );
  }

  private async measureWithCustomColors(): Promise<PerfMetrics> {
    return measurePerf(
      <Switch
        testID={this.testId}
        value={true}
        trackColor={{false: '#767577', true: '#81b0ff'}}
        thumbColor="#f5dd4b"
        ios_backgroundColor="#3e3e3e"
      />,
      {
        name: `${this.componentName} custom-colors`,
        runs: 10,
      },
    );
  }

  private async measureWithOnValueChange(): Promise<PerfMetrics> {
    return measurePerf(
      <Switch
        testID={this.testId}
        value={false}
        onValueChange={() => {}}
      />,
      {
        name: `${this.componentName} on-value-change`,
        runs: 10,
      },
    );
  }

  private async measureWithAccessibility(): Promise<PerfMetrics> {
    return measurePerf(
      <Switch
        testID={this.testId}
        value={false}
        accessibilityLabel="Enable notifications"
        accessibilityHint="Double tap to toggle"
        accessibilityRole="switch"
        accessibilityState={{checked: false}}
      />,
      {
        name: `${this.componentName} with-accessibility`,
        runs: 10,
      },
    );
  }

  private async measureMultipleSwitches(count: number): Promise<PerfMetrics> {
    const SwitchList = () => (
      <View style={styles.container}>
        {Array.from({length: count}, (_, i) => (
          <Switch
            key={i}
            value={i % 2 === 0}
            onValueChange={() => {}}
          />
        ))}
      </View>
    );

    return measurePerf(<SwitchList />, {
      name: `${this.componentName} multiple-${count}`,
      runs: 15,
    });
  }
}

const switchPerfTest = new SwitchPerfTest();

describe('Switch Performance', () => {
  test('mount time', async () => {
    const perf = await switchPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await switchPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await switchPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('Switch-Specific Scenarios', () => {
    test('value-true', async () => {
      const scenario = switchPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('disabled', async () => {
      const scenario = switchPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-custom-colors', async () => {
      const scenario = switchPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-on-value-change', async () => {
      const scenario = switchPerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-accessibility', async () => {
      const scenario = switchPerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-switches-10', async () => {
      const scenario = switchPerfTest.getCustomScenarios()[5];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('multiple-switches-50', async () => {
      const scenario = switchPerfTest.getCustomScenarios()[6];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({maxDurationIncrease: 15, minAbsoluteDelta: 5});
    });
  });
});

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  default: {},
});
