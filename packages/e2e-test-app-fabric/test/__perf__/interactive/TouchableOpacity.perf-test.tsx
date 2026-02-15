/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for TouchableOpacity component.
 *
 * @format
 */

import * as React from 'react';
import {TouchableOpacity, Text, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class TouchableOpacityPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'TouchableOpacity';
  readonly category = 'interactive' as const;
  readonly testId = 'perf-test-touchable-opacity';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <TouchableOpacity
        testID={this.testId}
        onPress={() => {}}
        style={styles.default}
        {...props}>
        <Text>Tap Me</Text>
      </TouchableOpacity>
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'custom-active-opacity',
        description: 'TouchableOpacity with custom activeOpacity',
        run: () => this.measureCustomActiveOpacity(),
      },
      {
        name: 'disabled',
        description: 'TouchableOpacity in disabled state',
        run: () => this.measureDisabled(),
      },
      {
        name: 'with-all-handlers',
        description: 'TouchableOpacity with onPressIn, onPressOut, onLongPress',
        run: () => this.measureWithAllHandlers(),
      },
      {
        name: 'with-hit-slop',
        description: 'TouchableOpacity with hitSlop expansion',
        run: () => this.measureWithHitSlop(),
      },
      {
        name: 'with-delay',
        description: 'TouchableOpacity with delayPressIn and delayPressOut',
        run: () => this.measureWithDelay(),
      },
      {
        name: 'nested-touchables',
        description: 'Nested TouchableOpacity components',
        run: () => this.measureNestedTouchables(),
      },
      {
        name: 'multiple-touchables-10',
        description: 'Render 10 sibling TouchableOpacity items',
        run: () => this.measureMultipleTouchables(10),
      },
      {
        name: 'multiple-touchables-50',
        description: 'Render 50 sibling TouchableOpacity items',
        run: () => this.measureMultipleTouchables(50),
      },
    ];
  }

  private async measureCustomActiveOpacity(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableOpacity
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        activeOpacity={0.5}>
        <Text>Custom Opacity</Text>
      </TouchableOpacity>,
      {
        name: `${this.componentName} custom-active-opacity`,
        runs: 10,
      },
    );
  }

  private async measureDisabled(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableOpacity
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        disabled={true}>
        <Text>Disabled</Text>
      </TouchableOpacity>,
      {
        name: `${this.componentName} disabled`,
        runs: 10,
      },
    );
  }

  private async measureWithAllHandlers(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableOpacity
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        onPressIn={() => {}}
        onPressOut={() => {}}
        onLongPress={() => {}}>
        <Text>All Handlers</Text>
      </TouchableOpacity>,
      {
        name: `${this.componentName} with-all-handlers`,
        runs: 10,
      },
    );
  }

  private async measureWithHitSlop(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableOpacity
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        hitSlop={{top: 20, bottom: 20, left: 20, right: 20}}>
        <Text>Hit Slop</Text>
      </TouchableOpacity>,
      {
        name: `${this.componentName} with-hit-slop`,
        runs: 10,
      },
    );
  }

  private async measureWithDelay(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableOpacity
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        delayPressIn={100}
        delayPressOut={100}
        delayLongPress={500}>
        <Text>With Delay</Text>
      </TouchableOpacity>,
      {
        name: `${this.componentName} with-delay`,
        runs: 10,
      },
    );
  }

  private async measureNestedTouchables(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableOpacity
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}>
        <TouchableOpacity style={styles.nested} onPress={() => {}}>
          <TouchableOpacity style={styles.nested} onPress={() => {}}>
            <Text>Deeply Nested</Text>
          </TouchableOpacity>
        </TouchableOpacity>
      </TouchableOpacity>,
      {
        name: `${this.componentName} nested`,
        runs: 10,
      },
    );
  }

  private async measureMultipleTouchables(count: number): Promise<PerfMetrics> {
    const TouchableList = () => (
      <View style={styles.container}>
        {Array.from({length: count}, (_, i) => (
          <TouchableOpacity key={i} style={styles.default} onPress={() => {}}>
            <Text>{`Item ${i}`}</Text>
          </TouchableOpacity>
        ))}
      </View>
    );

    return measurePerf(<TouchableList />, {
      name: `${this.componentName} multiple-${count}`,
      runs: 15,
    });
  }
}

const touchableOpacityPerfTest = new TouchableOpacityPerfTest();

describe('TouchableOpacity Performance', () => {
  test('mount time', async () => {
    const perf = await touchableOpacityPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await touchableOpacityPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await touchableOpacityPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('TouchableOpacity-Specific Scenarios', () => {
    test('custom-active-opacity', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('disabled', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-all-handlers', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-hit-slop', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-delay', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('nested-touchables', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[5];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-touchables-10', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[6];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('multiple-touchables-50', async () => {
      const scenario = touchableOpacityPerfTest.getCustomScenarios()[7];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({maxDurationIncrease: 15, minAbsoluteDelta: 5});
    });
  });
});

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  default: {
    padding: 10,
  },
  nested: {
    padding: 5,
    margin: 2,
  },
});
