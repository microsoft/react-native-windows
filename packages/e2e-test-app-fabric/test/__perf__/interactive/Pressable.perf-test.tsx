/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for Pressable component.
 *
 * @format
 */

import * as React from 'react';
import {Pressable, Text, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class PressablePerfTest extends ComponentPerfTestBase {
  readonly componentName = 'Pressable';
  readonly category = 'interactive' as const;
  readonly testId = 'perf-test-pressable';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <Pressable
        testID={this.testId}
        onPress={() => {}}
        style={styles.default}
        {...props}>
        <Text>Sample Content</Text>
      </Pressable>
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'with-all-handlers',
        description:
          'Pressable with onPressIn, onPressOut, onLongPress handlers',
        run: () => this.measureWithAllHandlers(),
      },
      {
        name: 'with-style-function',
        description: 'Pressable using a style function (dynamic pressed state)',
        run: () => this.measureWithStyleFunction(),
      },
      {
        name: 'disabled',
        description: 'Pressable in disabled state',
        run: () => this.measureDisabled(),
      },
      {
        name: 'with-hit-slop',
        description: 'Pressable with hitSlop expansion',
        run: () => this.measureWithHitSlop(),
      },
      {
        name: 'nested-pressables',
        description: 'Pressable nested inside another Pressable',
        run: () => this.measureNestedPressables(),
      },
      {
        name: 'multiple-pressables-10',
        description: 'Render 10 sibling Pressables',
        run: () => this.measureMultiplePressables(10),
      },
      {
        name: 'multiple-pressables-50',
        description: 'Render 50 sibling Pressables',
        run: () => this.measureMultiplePressables(50),
      },
    ];
  }

  private async measureWithAllHandlers(): Promise<PerfMetrics> {
    return measurePerf(
      <Pressable
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        onPressIn={() => {}}
        onPressOut={() => {}}
        onLongPress={() => {}}>
        <Text>All Handlers</Text>
      </Pressable>,
      {
        name: `${this.componentName} with-all-handlers`,
        runs: 10,
      },
    );
  }

  private async measureWithStyleFunction(): Promise<PerfMetrics> {
    return measurePerf(
      <Pressable
        testID={this.testId}
        onPress={() => {}}
        style={({pressed}) => [styles.default, pressed && styles.pressed]}>
        <Text>Style Function</Text>
      </Pressable>,
      {
        name: `${this.componentName} with-style-function`,
        runs: 10,
      },
    );
  }

  private async measureDisabled(): Promise<PerfMetrics> {
    return measurePerf(
      <Pressable
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        disabled={true}>
        <Text>Disabled</Text>
      </Pressable>,
      {
        name: `${this.componentName} disabled`,
        runs: 10,
      },
    );
  }

  private async measureWithHitSlop(): Promise<PerfMetrics> {
    return measurePerf(
      <Pressable
        testID={this.testId}
        style={styles.default}
        onPress={() => {}}
        hitSlop={{top: 20, bottom: 20, left: 20, right: 20}}>
        <Text>Hit Slop</Text>
      </Pressable>,
      {
        name: `${this.componentName} with-hit-slop`,
        runs: 10,
      },
    );
  }

  private async measureNestedPressables(): Promise<PerfMetrics> {
    return measurePerf(
      <Pressable testID={this.testId} style={styles.default} onPress={() => {}}>
        <Pressable style={styles.nested} onPress={() => {}}>
          <Pressable style={styles.nested} onPress={() => {}}>
            <Text>Deeply Nested</Text>
          </Pressable>
        </Pressable>
      </Pressable>,
      {
        name: `${this.componentName} nested`,
        runs: 10,
      },
    );
  }

  private async measureMultiplePressables(count: number): Promise<PerfMetrics> {
    const PressableList = () => (
      <View style={styles.container}>
        {Array.from({length: count}, (_, i) => (
          <Pressable key={i} style={styles.default} onPress={() => {}}>
            <Text>{`Item ${i}`}</Text>
          </Pressable>
        ))}
      </View>
    );

    return measurePerf(<PressableList />, {
      name: `${this.componentName} multiple-${count}`,
      runs: 15,
    });
  }
}

const pressablePerfTest = new PressablePerfTest();

describe('Pressable Performance', () => {
  test('mount time', async () => {
    const perf = await pressablePerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await pressablePerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await pressablePerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('Pressable-Specific Scenarios', () => {
    test('with-all-handlers', async () => {
      const scenario = pressablePerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-style-function', async () => {
      const scenario = pressablePerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('disabled', async () => {
      const scenario = pressablePerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-hit-slop', async () => {
      const scenario = pressablePerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('nested-pressables', async () => {
      const scenario = pressablePerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-pressables-10', async () => {
      const scenario = pressablePerfTest.getCustomScenarios()[5];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('multiple-pressables-50', async () => {
      const scenario = pressablePerfTest.getCustomScenarios()[6];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 15,
        minAbsoluteDelta: 5,
      });
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
  pressed: {
    opacity: 0.7,
    backgroundColor: '#ddd',
  },
  nested: {
    padding: 5,
    margin: 2,
  },
});
