# @react-native-windows/perf-testing

Performance testing utilities for React Native Windows components.

## Overview

This package provides infrastructure for measuring and tracking performance of React Native components. It enables:

- **Automated performance regression detection** on every PR
- **Component-wise performance baselines** (mount, unmount, re-render times)
- **Lightweight perf snapshots** similar to Jest snapshots
- **Easy extensibility** for new components via base classes

## Installation

```bash
npm install @react-native-windows/perf-testing --save-dev
# or
yarn add @react-native-windows/perf-testing --dev
```

## Quick Start

### 1. Create a Performance Test

```typescript
import { ComponentPerfTestBase, IScenario } from '@react-native-windows/perf-testing';
import { View } from 'react-native';

class ViewPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'View';
  readonly category = 'core' as const;
  readonly testId = 'perf-test-view';

  createComponent(props?: Record<string, unknown>) {
    return <View testID={this.testId} {...props} />;
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'nested-views',
        description: 'Test nested views performance',
        run: () => this.measureNestedViews(50),
      },
    ];
  }
}
```

### 2. Write Tests

```typescript
const viewPerfTest = new ViewPerfTest();

describe('View Performance', () => {
  test('mount time', async () => {
    const perf = await viewPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await viewPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });
});
```

### 3. Run Tests

```bash
# Run perf tests
yarn perf

# Update baselines
yarn perf:update
```

## API Reference

### Core Functions

| Function | Description |
|----------|-------------|
| `measurePerf(component, options)` | Core measurement function |
| `toMatchPerfSnapshot(threshold?)` | Jest matcher for perf snapshots |

### Base Classes

| Class | Description |
|-------|-------------|
| `ComponentPerfTestBase` | Abstract base class for component perf tests |

### Interfaces

| Interface | Description |
|-----------|-------------|
| `PerfMetrics` | Measurement results |
| `PerfThreshold` | Pass/fail thresholds |
| `IComponentPerfTest` | Contract for component tests |
| `IScenario` | Custom scenario interface |

### Threshold Presets

```typescript
import { ThresholdPresets } from '@react-native-windows/perf-testing';

// Available presets
ThresholdPresets.core       // 10% regression threshold
ThresholdPresets.list       // 15% for list components
ThresholdPresets.interactive // 20% for animated components
ThresholdPresets.community  // 25% relaxed threshold for external use
```

## Extending for Custom Components

Community developers can create perf tests for their custom native components:

```typescript
import { ComponentPerfTestBase } from '@react-native-windows/perf-testing';
import { MyCustomSlider } from 'my-custom-component';

class MyCustomSliderPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'MyCustomSlider';
  readonly category = 'custom' as const;
  readonly testId = 'perf-test-slider';

  createComponent(props?: Record<string, unknown>) {
    return <MyCustomSlider testID={this.testId} {...props} />;
  }
}
```

## Configuration

### Jest Setup

Add to your Jest setup file:

```typescript
import '@react-native-windows/perf-testing/matchers';
```

### Custom Thresholds

```typescript
expect(perf).toMatchPerfSnapshot({
  maxDurationIncrease: 15,  // Allow 15% regression
  maxRenderCount: 5,
});
```

## License

MIT
