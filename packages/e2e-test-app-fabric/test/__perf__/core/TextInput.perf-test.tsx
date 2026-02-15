/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for TextInput component.
 *
 * @format
 */

import * as React from 'react';
import {TextInput, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class TextInputPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'TextInput';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-textinput';

  get defaultThreshold() {
    return {
      maxDurationIncrease: 10,
      maxRenderCount: 3,
      minRuns: 10,
    };
  }

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <TextInput
        testID={this.testId}
        style={styles.default}
        placeholder="Enter text..."
        {...props}
      />
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'multiline',
        description: 'Multiline TextInput',
        run: () => this.measureMultiline(),
      },
      {
        name: 'with-value',
        description: 'TextInput with pre-filled value',
        run: () => this.measureWithValue(),
      },
      {
        name: 'styled-input',
        description: 'TextInput with custom styles',
        run: () => this.measureStyledInput(),
      },
    ];
  }

  private async measureMultiline(): Promise<PerfMetrics> {
    return measurePerf(
      <TextInput
        testID={this.testId}
        style={[styles.default, styles.multiline]}
        multiline={true}
        numberOfLines={4}
        placeholder="Enter multiline text..."
      />,
      {
        name: `${this.componentName} multiline`,
        runs: 10,
      },
    );
  }

  private async measureWithValue(): Promise<PerfMetrics> {
    const longValue = 'The quick brown fox jumps over the lazy dog. '.repeat(5);
    return measurePerf(
      <TextInput
        testID={this.testId}
        style={styles.default}
        value={longValue}
      />,
      {
        name: `${this.componentName} with-value`,
        runs: 10,
      },
    );
  }

  private async measureStyledInput(): Promise<PerfMetrics> {
    return measurePerf(
      <TextInput
        testID={this.testId}
        style={[styles.default, styles.styled]}
        placeholder="Styled input..."
      />,
      {
        name: `${this.componentName} styled`,
        runs: 10,
      },
    );
  }
}

const textInputPerfTest = new TextInputPerfTest();

describe('TextInput Performance', () => {
  test('mount time', async () => {
    const perf = await textInputPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await textInputPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await textInputPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('TextInput-Specific Scenarios', () => {
    test('multiline', async () => {
      const scenario = textInputPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-value', async () => {
      const scenario = textInputPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('styled-input', async () => {
      const scenario = textInputPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });
  });
});

const styles = StyleSheet.create({
  default: {
    height: 40,
    borderWidth: 1,
    borderColor: '#ccc',
    paddingHorizontal: 8,
  },
  multiline: {
    height: 100,
    textAlignVertical: 'top',
  },
  styled: {
    fontSize: 16,
    fontWeight: 'bold',
    color: 'navy',
    backgroundColor: '#f0f0f0',
    borderRadius: 8,
    borderColor: 'blue',
    borderWidth: 2,
  },
});
