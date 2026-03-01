/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for Image component.
 *
 * @format
 */

import * as React from 'react';
import {Image, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

const TEST_URI = 'https://example.com/img.png';

class ImagePerfTest extends ComponentPerfTestBase {
  readonly componentName = 'Image';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-image';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <Image
        testID={this.testId}
        source={{uri: TEST_URI}}
        style={styles.default}
        {...props}
      />
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'with-resize-mode',
        description: 'Image with resizeMode="contain"',
        run: () => this.measureWithResizeMode(),
      },
      {
        name: 'with-border-radius',
        description: 'Image with rounded corners',
        run: () => this.measureWithBorderRadius(),
      },
      {
        name: 'with-tint-color',
        description: 'Image with tintColor applied',
        run: () => this.measureWithTintColor(),
      },
      {
        name: 'with-blur-radius',
        description: 'Image with blurRadius',
        run: () => this.measureWithBlurRadius(),
      },
      {
        name: 'with-accessibility',
        description: 'Image with full accessibility props',
        run: () => this.measureWithAccessibility(),
      },
      {
        name: 'multiple-images-10',
        description: 'Render 10 sibling Images',
        run: () => this.measureMultipleImages(10),
      },
      {
        name: 'multiple-images-50',
        description: 'Render 50 sibling Images',
        run: () => this.measureMultipleImages(50),
      },
      {
        name: 'multiple-images-100',
        description: 'Render 100 sibling Images (stress gate)',
        run: () => this.measureMultipleImages(100),
      },
    ];
  }

  private async measureWithResizeMode(): Promise<PerfMetrics> {
    return measurePerf(
      <Image
        testID={this.testId}
        source={{uri: TEST_URI}}
        style={styles.default}
        resizeMode="contain"
      />,
      {
        name: `${this.componentName} with-resize-mode`,
        runs: 10,
      },
    );
  }

  private async measureWithBorderRadius(): Promise<PerfMetrics> {
    return measurePerf(
      <Image
        testID={this.testId}
        source={{uri: TEST_URI}}
        style={[styles.default, styles.rounded]}
      />,
      {
        name: `${this.componentName} with-border-radius`,
        runs: 10,
      },
    );
  }

  private async measureWithTintColor(): Promise<PerfMetrics> {
    return measurePerf(
      <Image
        testID={this.testId}
        source={{uri: TEST_URI}}
        style={[styles.default, styles.tinted]}
      />,
      {
        name: `${this.componentName} with-tint-color`,
        runs: 10,
      },
    );
  }

  private async measureWithBlurRadius(): Promise<PerfMetrics> {
    return measurePerf(
      <Image
        testID={this.testId}
        source={{uri: TEST_URI}}
        style={styles.default}
        blurRadius={5}
      />,
      {
        name: `${this.componentName} with-blur-radius`,
        runs: 10,
      },
    );
  }

  private async measureWithAccessibility(): Promise<PerfMetrics> {
    return measurePerf(
      <Image
        testID={this.testId}
        source={{uri: TEST_URI}}
        style={styles.default}
        accessible={true}
        accessibilityLabel="Profile photo"
        accessibilityRole="image"
      />,
      {
        name: `${this.componentName} with-accessibility`,
        runs: 10,
      },
    );
  }

  private async measureMultipleImages(count: number): Promise<PerfMetrics> {
    const ImageList = () => (
      <View style={styles.container}>
        {Array.from({length: count}, (_, i) => (
          <Image
            key={i}
            source={{uri: `${TEST_URI}?i=${i}`}}
            style={styles.thumbnail}
          />
        ))}
      </View>
    );

    return measurePerf(<ImageList />, {
      name: `${this.componentName} multiple-${count}`,
      runs: 15,
    });
  }
}

const imagePerfTest = new ImagePerfTest();

describe('Image Performance', () => {
  test('mount time', async () => {
    const perf = await imagePerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await imagePerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await imagePerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('Image-Specific Scenarios', () => {
    test('with-resize-mode', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[0];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-border-radius', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[1];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-tint-color', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[2];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-blur-radius', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[3];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-accessibility', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[4];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('multiple-images-10', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[5];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('multiple-images-50', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[6];
      const perf = await scenario.run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 15,
        minAbsoluteDelta: 5,
      });
    });

    test('multiple-images-100', async () => {
      const scenario = imagePerfTest.getCustomScenarios()[7];
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
  default: {
    width: 100,
    height: 100,
  },
  thumbnail: {
    width: 50,
    height: 50,
    margin: 2,
  },
  rounded: {
    borderRadius: 50,
  },
  tinted: {
    tintColor: '#841584',
  },
});
