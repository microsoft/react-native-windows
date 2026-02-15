/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for Modal component.
 *
 * @format
 */

import * as React from 'react';
import {Modal, View, Text, Button, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class ModalPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'Modal';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-modal';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <Modal
        testID={this.testId}
        visible={true}
        {...props}>
        <View style={styles.centeredView}>
          <Text>Modal Content</Text>
        </View>
      </Modal>
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'slide-animation',
        description: 'Modal with slide animation type',
        run: () => this.measureSlideAnimation(),
      },
      {
        name: 'fade-animation',
        description: 'Modal with fade animation type',
        run: () => this.measureFadeAnimation(),
      },
      {
        name: 'transparent',
        description: 'Modal with transparent background',
        run: () => this.measureTransparent(),
      },
      {
        name: 'with-callbacks',
        description: 'Modal with onShow, onRequestClose, onDismiss handlers',
        run: () => this.measureWithCallbacks(),
      },
      {
        name: 'with-rich-content',
        description: 'Modal with complex child content (form-like)',
        run: () => this.measureWithRichContent(),
      },
      {
        name: 'with-accessibility',
        description: 'Modal with full accessibility props',
        run: () => this.measureWithAccessibility(),
      },
    ];
  }

  private async measureSlideAnimation(): Promise<PerfMetrics> {
    return measurePerf(
      <Modal testID={this.testId} visible={true} animationType="slide">
        <View style={styles.centeredView}>
          <Text>Slide Modal</Text>
        </View>
      </Modal>,
      {
        name: `${this.componentName} slide-animation`,
        runs: 10,
      },
    );
  }

  private async measureFadeAnimation(): Promise<PerfMetrics> {
    return measurePerf(
      <Modal testID={this.testId} visible={true} animationType="fade">
        <View style={styles.centeredView}>
          <Text>Fade Modal</Text>
        </View>
      </Modal>,
      {
        name: `${this.componentName} fade-animation`,
        runs: 10,
      },
    );
  }

  private async measureTransparent(): Promise<PerfMetrics> {
    return measurePerf(
      <Modal testID={this.testId} visible={true} transparent={true}>
        <View style={styles.overlay}>
          <View style={styles.dialog}>
            <Text>Transparent Modal</Text>
          </View>
        </View>
      </Modal>,
      {
        name: `${this.componentName} transparent`,
        runs: 10,
      },
    );
  }

  private async measureWithCallbacks(): Promise<PerfMetrics> {
    return measurePerf(
      <Modal
        testID={this.testId}
        visible={true}
        onShow={() => {}}
        onRequestClose={() => {}}
        onDismiss={() => {}}>
        <View style={styles.centeredView}>
          <Text>Modal with Callbacks</Text>
        </View>
      </Modal>,
      {
        name: `${this.componentName} with-callbacks`,
        runs: 10,
      },
    );
  }

  private async measureWithRichContent(): Promise<PerfMetrics> {
    const RichModal = () => (
      <Modal testID={this.testId} visible={true}>
        <View style={styles.centeredView}>
          <View style={styles.dialog}>
            <Text style={styles.title}>Dialog Title</Text>
            <Text style={styles.body}>
              This is a longer description that simulates real modal content
              with multiple lines of text.
            </Text>
            <View style={styles.buttonRow}>
              <Button title="Cancel" onPress={() => {}} />
              <Button title="Confirm" onPress={() => {}} />
            </View>
          </View>
        </View>
      </Modal>
    );

    return measurePerf(<RichModal />, {
      name: `${this.componentName} rich-content`,
      runs: 10,
    });
  }

  private async measureWithAccessibility(): Promise<PerfMetrics> {
    return measurePerf(
      <Modal
        testID={this.testId}
        visible={true}
        accessibilityLabel="Confirmation dialog"
        accessibilityViewIsModal={true}>
        <View style={styles.centeredView}>
          <Text>Accessible Modal</Text>
        </View>
      </Modal>,
      {
        name: `${this.componentName} with-accessibility`,
        runs: 10,
      },
    );
  }
}

const modalPerfTest = new ModalPerfTest();

describe('Modal Performance', () => {
  test('mount time', async () => {
    const perf = await modalPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await modalPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await modalPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('Modal-Specific Scenarios', () => {
    test('slide-animation', async () => {
      const scenario = modalPerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('fade-animation', async () => {
      const scenario = modalPerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('transparent', async () => {
      const scenario = modalPerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-callbacks', async () => {
      const scenario = modalPerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-rich-content', async () => {
      const scenario = modalPerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-accessibility', async () => {
      const scenario = modalPerfTest.getCustomScenarios()[5];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });
  });
});

const styles = StyleSheet.create({
  centeredView: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  overlay: {
    flex: 1,
    backgroundColor: 'rgba(0, 0, 0, 0.5)',
    justifyContent: 'center',
    alignItems: 'center',
  },
  dialog: {
    backgroundColor: 'white',
    borderRadius: 10,
    padding: 20,
    width: 300,
    alignItems: 'center',
    elevation: 5,
  },
  title: {
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: 10,
  },
  body: {
    fontSize: 14,
    textAlign: 'center',
    marginBottom: 20,
  },
  buttonRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    width: '100%',
  },
});
