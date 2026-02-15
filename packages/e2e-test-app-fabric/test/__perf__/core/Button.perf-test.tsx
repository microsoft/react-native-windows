/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for Button component.
 *
 * @format
 */

import * as React from 'react';
import {Button, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class ButtonPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'Button';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-button';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <Button
        testID={this.testId}
        title="Perf Test Button"
        onPress={() => {}}
        {...props}
      />
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'disabled',
        description: 'Button in disabled state',
        run: () => this.measureDisabled(),
      },
      {
        name: 'with-color',
        description: 'Button with custom color prop',
        run: () => this.measureWithColor(),
      },
      {
        name: 'with-accessibility',
        description: 'Button with full accessibility props',
        run: () => this.measureWithAccessibility(),
      },
      {
        name: 'multiple-buttons-10',
        description: 'Render 10 sibling Buttons',
        run: () => this.measureMultipleButtons(10),
      },
      {
        name: 'multiple-buttons-50',
        description: 'Render 50 sibling Buttons',
        run: () => this.measureMultipleButtons(50),
      },
    ];
  }

  private async measureDisabled(): Promise<PerfMetrics> {
    return measurePerf(
      <Button
        testID={this.testId}
        title="Disabled Button"
        onPress={() => {}}
        disabled={true}
      />,
      {
        name: `${this.componentName} disabled`,
        runs: 10,
      },
    );
  }

  private async measureWithColor(): Promise<PerfMetrics> {
    return measurePerf(
      <Button
        testID={this.testId}
        title="Colored Button"
        onPress={() => {}}
        color="#841584"
      />,
      {
        name: `${this.componentName} with-color`,
        runs: 10,
      },
    );
  }

  private async measureWithAccessibility(): Promise<PerfMetrics> {
    return measurePerf(
      <Button
        testID={this.testId}
        title="Accessible Button"
        onPress={() => {}}
        accessibilityLabel="Submit form"
        accessibilityHint="Double tap to submit the form"
        accessibilityState={{disabled: false}}
      />,
      {
        name: `${this.componentName} with-accessibility`,
        runs: 10,
      },
    );
  }

  private async measureMultipleButtons(count: number): Promise<PerfMetrics> {
    const ButtonList = () => (
      <View style={styles.container}>
        {Array.from({length: count}, (_, i) => (
          <Button
            key={i}
            title={`Button ${i}`}
            onPress={() => {}}
          />
        ))}
      </View>
    );

    return measurePerf(<ButtonList />, {
      name: `${this.componentName} multiple-${count}`,
      runs: 15,
    });
  }
}

// ─── TEST SUITE ───

const buttonPerfTest = new ButtonPerfTest();

describe('Button Performance', () => {
  test('mount time', async () => {
    const perf = await buttonPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await buttonPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await buttonPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('Button-Specific Scenarios', () => {
    test('disabled', async () => {
      const scenario = buttonPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-color', async () => {
      const scenario = buttonPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-accessibility', async () => {
      const scenario = buttonPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-buttons-10', async () => {
      const scenario = buttonPerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('multiple-buttons-50', async () => {
      const scenario = buttonPerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({maxDurationIncrease: 15, minAbsoluteDelta: 5});
    });
  });
});

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
});
