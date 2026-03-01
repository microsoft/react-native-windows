/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for TouchableHighlight component.
 *
 * @format
 */

import * as React from 'react';
import {TouchableHighlight, Text, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class TouchableHighlightPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'TouchableHighlight';
  readonly category = 'interactive' as const;
  readonly testId = 'perf-test-touchable-highlight';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <TouchableHighlight
        testID={this.testId}
        onPress={() => {}}
        style={styles.default}
        {...props}>
        <Text>Perf Test</Text>
      </TouchableHighlight>
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'custom-underlay-color',
        description: 'TouchableHighlight with custom underlay color',
        run: () => this.measureCustomUnderlayColor(),
      },
      {
        name: 'custom-active-opacity',
        description: 'TouchableHighlight with custom active opacity',
        run: () => this.measureCustomActiveOpacity(),
      },
      {
        name: 'disabled',
        description: 'TouchableHighlight in disabled state',
        run: () => this.measureDisabled(),
      },
      {
        name: 'with-all-handlers',
        description: 'TouchableHighlight with all event handlers attached',
        run: () => this.measureWithAllHandlers(),
      },
      {
        name: 'with-hit-slop',
        description: 'TouchableHighlight with custom hit slop',
        run: () => this.measureWithHitSlop(),
      },
      {
        name: 'nested-touchables',
        description: 'Nested TouchableHighlight components',
        run: () => this.measureNestedTouchables(),
      },
      {
        name: 'multiple-touchables-10',
        description: 'Render 10 TouchableHighlight components',
        run: () => this.measureMultipleTouchables(10),
      },
      {
        name: 'multiple-touchables-50',
        description: 'Render 50 TouchableHighlight components',
        run: () => this.measureMultipleTouchables(50),
      },
      {
        name: 'multiple-touchables-100',
        description: 'Render 100 TouchableHighlight components (stress gate)',
        run: () => this.measureMultipleTouchables(100),
      },
    ];
  }

  private async measureCustomUnderlayColor(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableHighlight
        testID={this.testId}
        onPress={() => {}}
        underlayColor="#FF6347"
        style={styles.default}>
        <Text>Underlay Color</Text>
      </TouchableHighlight>,
      {name: `${this.componentName} custom-underlay-color`, runs: 10},
    );
  }

  private async measureCustomActiveOpacity(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableHighlight
        testID={this.testId}
        onPress={() => {}}
        activeOpacity={0.5}
        underlayColor="#DDDDDD"
        style={styles.default}>
        <Text>Active Opacity</Text>
      </TouchableHighlight>,
      {name: `${this.componentName} custom-active-opacity`, runs: 10},
    );
  }

  private async measureDisabled(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableHighlight
        testID={this.testId}
        onPress={() => {}}
        disabled={true}
        style={styles.disabled}>
        <Text>Disabled</Text>
      </TouchableHighlight>,
      {name: `${this.componentName} disabled`, runs: 10},
    );
  }

  private async measureWithAllHandlers(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableHighlight
        testID={this.testId}
        onPress={() => {}}
        onPressIn={() => {}}
        onPressOut={() => {}}
        onLongPress={() => {}}
        onShowUnderlay={() => {}}
        onHideUnderlay={() => {}}
        style={styles.default}>
        <Text>All Handlers</Text>
      </TouchableHighlight>,
      {name: `${this.componentName} with-all-handlers`, runs: 10},
    );
  }

  private async measureWithHitSlop(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableHighlight
        testID={this.testId}
        onPress={() => {}}
        hitSlop={{top: 20, bottom: 20, left: 20, right: 20}}
        style={styles.default}>
        <Text>Hit Slop</Text>
      </TouchableHighlight>,
      {name: `${this.componentName} with-hit-slop`, runs: 10},
    );
  }

  private async measureNestedTouchables(): Promise<PerfMetrics> {
    return measurePerf(
      <TouchableHighlight
        testID={this.testId}
        onPress={() => {}}
        underlayColor="#EEEEEE"
        style={styles.outer}>
        <View>
          <TouchableHighlight
            onPress={() => {}}
            underlayColor="#DDDDDD"
            style={styles.inner}>
            <View>
              <TouchableHighlight
                onPress={() => {}}
                underlayColor="#CCCCCC"
                style={styles.innermost}>
                <Text>Nested</Text>
              </TouchableHighlight>
            </View>
          </TouchableHighlight>
        </View>
      </TouchableHighlight>,
      {name: `${this.componentName} nested-touchables`, runs: 10},
    );
  }

  private async measureMultipleTouchables(count: number): Promise<PerfMetrics> {
    const items = Array.from({length: count}, (_, i) => (
      <TouchableHighlight
        key={i}
        onPress={() => {}}
        underlayColor="#DDDDDD"
        style={styles.item}>
        <Text>{`Item ${i}`}</Text>
      </TouchableHighlight>
    ));
    return measurePerf(<View testID={this.testId}>{items}</View>, {
      name: `${this.componentName} multiple-touchables-${count}`,
      runs: 10,
    });
  }
}

const touchableHighlightPerfTest = new TouchableHighlightPerfTest();

describe('TouchableHighlight Performance', () => {
  test('mount time', async () => {
    const perf = await touchableHighlightPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await touchableHighlightPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await touchableHighlightPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('TouchableHighlight-Specific Scenarios', () => {
    const scenarios = touchableHighlightPerfTest.getCustomScenarios();

    test('custom-underlay-color', async () => {
      const perf = await scenarios[0].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('custom-active-opacity', async () => {
      const perf = await scenarios[1].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('disabled', async () => {
      const perf = await scenarios[2].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-all-handlers', async () => {
      const perf = await scenarios[3].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-hit-slop', async () => {
      const perf = await scenarios[4].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('nested-touchables', async () => {
      const perf = await scenarios[5].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-touchables-10', async () => {
      const perf = await scenarios[6].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('multiple-touchables-50', async () => {
      const perf = await scenarios[7].run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 15,
        minAbsoluteDelta: 5,
      });
    });

    test('multiple-touchables-100', async () => {
      const perf = await scenarios[8].run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 10,
        minAbsoluteDelta: 10,
        mode: 'gate',
      });
    });
  });
});

const styles = StyleSheet.create({
  default: {
    padding: 10,
  },
  disabled: {
    padding: 10,
    opacity: 0.5,
  },
  outer: {
    padding: 20,
  },
  inner: {
    padding: 15,
  },
  innermost: {
    padding: 10,
  },
  item: {
    padding: 8,
    marginVertical: 2,
  },
});
