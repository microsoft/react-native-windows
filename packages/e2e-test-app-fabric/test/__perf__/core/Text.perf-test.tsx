/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for Text component.
 *
 * @format
 */

import * as React from 'react';
import {Text, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class TextPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'Text';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-text';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <Text testID={this.testId} style={styles.default} {...props}>
        Sample Text
      </Text>
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'long-text',
        description: 'Text with 1000 characters',
        run: () => this.measureLongText(1000),
      },
      {
        name: 'nested-text',
        description: 'Nested Text components',
        run: () => this.measureNestedText(),
      },
      {
        name: 'styled-text',
        description: 'Text with multiple styles',
        run: () => this.measureStyledText(),
      },
      {
        name: 'multiple-text-100',
        description: 'Render 100 sibling Text components (stress gate)',
        run: () => this.measureMultipleTexts(100),
      },
    ];
  }

  private async measureLongText(length: number): Promise<PerfMetrics> {
    const longString = 'A'.repeat(length);
    return measurePerf(<Text testID={this.testId}>{longString}</Text>, {
      name: `${this.componentName} long-${length}`,
      runs: 10,
    });
  }

  private async measureNestedText(): Promise<PerfMetrics> {
    return measurePerf(
      <Text testID={this.testId} style={styles.default}>
        Outer text{' '}
        <Text style={styles.bold}>
          bold text <Text style={styles.italic}>italic text</Text>
        </Text>
      </Text>,
      {
        name: `${this.componentName} nested`,
        runs: 10,
      },
    );
  }

  private async measureStyledText(): Promise<PerfMetrics> {
    return measurePerf(
      <Text testID={this.testId} style={[styles.default, styles.styled]}>
        Styled Text
      </Text>,
      {
        name: `${this.componentName} styled`,
        runs: 10,
      },
    );
  }

  private async measureMultipleTexts(count: number): Promise<PerfMetrics> {
    const TextList = () => (
      <View testID={this.testId}>
        {Array.from({length: count}, (_, i) => (
          <Text key={i} style={styles.default}>
            {`Text item ${i} with some content`}
          </Text>
        ))}
      </View>
    );

    return measurePerf(<TextList />, {
      name: `${this.componentName} multiple-${count}`,
      runs: 15,
    });
  }
}

const textPerfTest = new TextPerfTest();

describe('Text Performance', () => {
  test('mount time', async () => {
    const perf = await textPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await textPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await textPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('Text-Specific Scenarios', () => {
    test('long-text', async () => {
      const scenario = textPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('nested-text', async () => {
      const scenario = textPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('styled-text', async () => {
      const scenario = textPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-text-100', async () => {
      const scenario = textPerfTest.getCustomScenarios()[3];
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
  default: {
    fontSize: 14,
  },
  bold: {
    fontWeight: 'bold',
  },
  italic: {
    fontStyle: 'italic',
  },
  styled: {
    fontSize: 18,
    fontWeight: 'bold',
    color: 'blue',
    textDecorationLine: 'underline',
  },
});
