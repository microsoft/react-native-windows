# Component Performance Tests for React Native Windows (Fabric)

## Summary

Adds a comprehensive performance testing framework and 14 component-level perf test suites (147 tests total) for RNW Fabric components. Each test measures mount, unmount, and re-render times and records baselines via snapshot matching.

## Why

RNW Fabric had no automated way to catch render performance regressions in core components. This adds baseline-tracked perf tests so regressions are caught before merge.

## What's Included

### Perf Testing Framework (`@react-native-windows/perf-testing`)

| Module | Purpose |
|---|---|
| `ComponentPerfTestBase` | Abstract base class — components provide props/scenarios, framework handles measurement |
| `measurePerf()` | Core timing engine using `React.Profiler` |
| `toMatchPerfSnapshot()` | Custom Jest matcher — compares results against `.perf-baseline.json` snapshots |
| `PerfProfiler` | React wrapper that captures `actualDuration` from Profiler callbacks |
| `snapshotManager` | Reads/writes/updates baseline JSON files |
| Threshold presets | `strict`, `standard`, `relaxed`, `ci` — configurable tolerance bands |
| Scenarios | `MountScenario`, `UnmountScenario`, `RerenderScenario` |
| Reporters | `ConsoleReporter`, `MarkdownReporter`, `PerfJsonReporter` |
| CI utilities | `BaselineComparator` for CI regression detection |

### Component Test Suites (14 suites, 147 tests)

| Category | Component | Tests | Key Scenarios |
|---|---|---|---|
| **Core** | View | 8 | default, nested, styled, accessible, nativeID, testID, pointerEvents, **stress-views-500** |
| | Text | 7 | default, long text, nested, styled, selectable, accessible, **multiple-text-100** |
| | TextInput | 7 | default, placeholder, multiline, secure, styled, accessible, **multiple-text-inputs-100** |
| | Button | 9 | default, colored, disabled, accessible, long title, custom a11y, styled container, all props, **multiple-buttons-100** |
| | Image | 11 | default, remote URI, sized, resizeMode, accessible, tinted, background, blurred, multiple sources, all props, **multiple-images-100** |
| | ScrollView | 11 | default, horizontal, many children, pagingEnabled, styled, accessible, stickyHeaders, nestedScroll, invertedStickyHeaders, contentInset, **with-children-500** |
| | Switch | 11 | default, enabled-on, disabled, custom colors, accessible, styled, ios_backgroundColor, all props, toggled-on, toggled-off, **multiple-switches-100** |
| | Modal | 9 | default, visible, transparent, slide, fade, fullscreen, accessible, onShow, all props |
| | ActivityIndicator | 11 | default, animating, large, custom color, small, styled, accessible, non-animating, custom size, all props, **multiple-indicators-100** |
| **Interactive** | Pressable | 11 | default, disabled, styled, accessible, pressed-style, ripple, hitSlop, delayLongPress, unstable_pressDelay, all props, **multiple-pressables-100** |
| | TouchableOpacity | 12 | default, active-opacity, disabled, styled, accessible, long-press, press-in/out, hitSlop, custom-a11y, all-props, focus, **multiple-touchables-100** |
| | TouchableHighlight | 12 | default, custom-underlay, opacity, disabled, styled, accessible, long-press, show/hide-underlay, hitSlop, all-props, focus, **multiple-touchables-100** |
| **Lists** | FlatList | 14 | 10/100/500/***1000*** items, horizontal, separator, header+footer, empty, getItemLayout, inverted, numColumns(3), sections |
| | SectionList | 14 | 3×5, 5×10, 10×20, 20×10, **50×20** sections, separators, header+footer, section-footer, sticky-headers, empty, inverted, renderSectionHeader, all-features |

### CLI Generator

`yarn perf:create -- --name=ComponentName` scaffolds a new `.perf-test.tsx` file with the correct structure, required props detection, and category selection.

### Scripts

- `yarn perf` — run all perf tests
- `yarn perf:update` — update baselines
- `yarn perf:create` — scaffold a new test

## Folder Structure

```
packages/e2e-test-app-fabric/
├── jest.perf.config.js                          # Jest config (maxWorkers:1, .perf-test pattern)
├── jest.perf.setup.ts                           # Test setup (registers toMatchPerfSnapshot matcher)
└── test/__perf__/
    ├── core/                                    # 9 core component tests
    │   ├── View.perf-test.tsx
    │   ├── Text.perf-test.tsx
    │   ├── TextInput.perf-test.tsx
    │   ├── Button.perf-test.tsx
    │   ├── Image.perf-test.tsx
    │   ├── ScrollView.perf-test.tsx
    │   ├── Switch.perf-test.tsx
    │   ├── Modal.perf-test.tsx
    │   ├── ActivityIndicator.perf-test.tsx
    │   └── __perf_snapshots__/                  # Baseline JSONs (one per test)
    ├── interactive/                             # 3 interactive component tests
    │   ├── Pressable.perf-test.tsx
    │   ├── TouchableOpacity.perf-test.tsx
    │   ├── TouchableHighlight.perf-test.tsx
    │   └── __perf_snapshots__/
    └── list/                                    # 2 list component tests
        ├── FlatList.perf-test.tsx
        ├── SectionList.perf-test.tsx
        └── __perf_snapshots__/

packages/@react-native-windows/perf-testing/src/
├── index.ts                                     # Public API exports
├── base/
│   └── ComponentPerfTestBase.ts                 # Abstract base class for tests
├── core/
│   ├── measurePerf.ts                           # Timing engine (React.Profiler + performance.now)
│   ├── PerfProfiler.tsx                         # React.Profiler wrapper
│   └── statistics.ts                            # mean, median, stdDev
├── interfaces/
│   ├── IComponentPerfTest.ts                    # Test interface contract
│   ├── PerfMetrics.ts                           # Result shape
│   └── PerfThreshold.ts                         # Threshold config shape
├── matchers/
│   ├── toMatchPerfSnapshot.ts                   # Custom Jest matcher
│   └── snapshotManager.ts                       # Baseline file read/write
├── scenarios/
│   ├── MountScenario.ts
│   ├── UnmountScenario.ts
│   └── RerenderScenario.ts
├── config/
│   ├── defaultConfig.ts                         # Default runs, warmup, thresholds
│   └── thresholdPresets.ts                      # strict / standard / relaxed / ci
├── reporters/
│   ├── ConsoleReporter.ts                       # Terminal output
│   └── MarkdownReporter.ts                      # .md report generation
└── ci/
    ├── PerfJsonReporter.ts                      # JSON results for CI artifacts
    └── BaselineComparator.ts                    # Regression detection

vnext/Scripts/perf/
├── create-perf-test.js                          # CLI scaffold generator (yarn perf:create)
├── compare-results.js                           # CI baseline comparison
└── post-pr-comment.js                           # GitHub PR comment poster
```

## Test Architecture

```
ComponentPerfTest extends ComponentPerfTestBase
  ├── componentName()   → 'FlatList'
  ├── baseProps()       → minimal valid props
  ├── scenarios()       → [{ name, props, description }]
  └── (optional) renderChildren(), wrapComponent()

measurePerf(test, scenario)
  ├── renders via PerfProfiler
  ├── captures actualDuration from React.Profiler
  ├── runs mount / unmount / rerender cycles
  └── returns PerfMetrics

expect(metrics).toMatchPerfSnapshot()
  ├── loads/creates .perf-baseline.json
  ├── compares against thresholds
  └── updates baseline when --updateSnapshot
```

## How to Run

```bash
cd packages/e2e-test-app-fabric
yarn perf                                              # all tests
yarn perf -- --testPathPattern=FlatList                 # single component
yarn perf:update                                       # update all baselines
yarn perf:update --testPathPattern=TouchableHighlight   # update one baseline
```

## Release Notes

Added `@react-native-windows/perf-testing`, a component performance testing framework for RNW Fabric. Includes 147 perf tests across 14 components (View, Text, TextInput, Button, Image, ScrollView, Switch, Modal, ActivityIndicator, Pressable, TouchableOpacity, TouchableHighlight, FlatList, SectionList) measuring mount, unmount, and re-render times with snapshot-based baseline tracking. Run `yarn perf` to execute and `yarn perf:create` to scaffold tests for new components.

---

## Statistical Stability Model

Simple median-vs-median comparison is unreliable at millisecond scale — system noise causes random failures. We use three gates, each proven in production at scale:

1. **CV Gate** — If coefficient of variation (`stdDev/mean`) exceeds threshold, the measurement is too noisy to compare; warn instead of fail.
2. **Mann-Whitney U Test** — Non-parametric rank-based hypothesis test on raw `durations[]`. Only flags a regression when the difference is statistically significant (p < 0.05), not just numerically larger. Chosen over t-test because perf data is rarely normally distributed.
3. **Gate / Track Mode** — Stable tests use `gate` (block CI). Inherently variable bulk scenarios use `track` (warn only, never block).

### Industry References

| Who | What | Link |
|---|---|---|
| **Google** | Chrome Catapult uses Mann-Whitney U in `getDifferenceSignificance()` to detect perf regressions across histogram samples | [chromium.googlesource.com/catapult — How to Write Metrics](https://chromium.googlesource.com/catapult/+/HEAD/docs/how-to-write-metrics.md) |
| **Microsoft** | BenchmarkDotNet uses CV-based noise detection and statistical significance gates to suppress unstable benchmark results | [benchmarkdotnet.org — How It Works](https://benchmarkdotnet.org/articles/guides/how-it-works.html) |
| **Meta** | React Profiler `actualDuration` provides component-level render timing — the same API this framework builds on | [react.dev — Profiler API](https://react.dev/reference/react/Profiler) |

---

## Benchmark Gates & Regression Thresholds

### How a regression is detected

Every test result passes through this decision pipeline (in `toMatchPerfSnapshot` and `BaselineComparator`):

```
measured durations[]
       │
       ▼
  CV > maxCV?  ──yes──▶  SKIP (too noisy to judge) → warn only
       │ no
       ▼
  Mann-Whitney U
  p ≥ 0.05?   ──yes──▶  PASS (not statistically significant)
       │ no
       ▼
  % change > maxDurationIncrease%
  AND absolute Δ > minAbsoluteDelta ms?i
       │
    yes │ no
       ▼    ▼
  mode?    PASS
   │
  gate ──▶ FAIL CI
  track ─▶ WARN only
```

Both the **percentage** and **absolute delta** gates must trip simultaneously. This prevents a 1 ms → 2 ms jump (100 % but only 1 ms) from blocking CI.

### Threshold Presets

| Preset | Max Regression | Min Δ (ms) | Max Renders | Min Runs | Max CV | Mode |
|--------|---------------|------------|-------------|----------|--------|------|
| **core** | 10 % | 3 | 2 | 10 | 0.40 | gate |
| **list** | 15 % | 5 | 5 | 5 | 0.50 | gate |
| **interactive** | 20 % | 5 | 10 | 10 | 0.50 | gate |
| **community** | 25 % | 5 | 15 | 5 | 0.60 | track |
| **default** | 10 % | 3 | 5 | 10 | 0.50 | gate |

### Per-Component Benchmark Map

Tests use the **default** threshold (10 % / 3 ms / gate) unless stated otherwise. The table below lists every scenario with its baseline median, the effective gate, and the absolute delta floor.

#### Core Components (9 suites)

| Component | Scenario | Baseline (ms) | Max Regression | Min Δ (ms) | Notes |
|---|---|---:|---|---:|---|
| **View** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0 | 10 % | 3 | |
| | nested-views-50 | 4 | **15 %** | **5** | heavier DOM |
| | nested-views-100 | 7 | **15 %** | **5** | heavier DOM |
| | **stress-views-500** | **10** | **10 %** | **10** | **stress gate** |
| | with-shadow | 0 | 10 % | 3 | |
| | with-border-radius | 0 | 10 % | 3 | |
| **Text** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0 | 10 % | 3 | |
| | long-text-1000 | 0 | 10 % | 3 | |
| | nested-text | 0 | 10 % | 3 | |
| | styled-text | 0 | 10 % | 3 | |
| | **multiple-text-100** | **7** | **10 %** | **10** | **stress gate** |
| **Image** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0 | 10 % | 3 | |
| | with-resize-mode | 0 | 10 % | 3 | |
| | with-border-radius | 0 | 10 % | 3 | |
| | with-tint-color | 0 | 10 % | 3 | |
| | with-blur-radius | 0 | 10 % | 3 | |
| | with-accessibility | 0 | 10 % | 3 | |
| | multiple-images-10 | 1 | 10 % | **5** | bulk noise |
| | multiple-images-50 | 4 | **15 %** | **5** | bulk noise |
| | **multiple-images-100** | **8** | **10 %** | **10** | **stress gate** |
| **TextInput** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0 | 10 % | 3 | |
| | multiline | 0 | 10 % | 3 | |
| | with-value | 0 | 10 % | 3 | |
| | styled-input | 0 | 10 % | 3 | |
| | **multiple-text-inputs-100** | **7** | **10 %** | **10** | **stress gate** |
| **Switch** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0 | 10 % | 3 | |
| | disabled | 0.5 | 10 % | 3 | |
| | custom-colors | 0 | 10 % | 3 | |
| | multiple-switches-10 | 1 | 10 % | **5** | bulk noise |
| | multiple-switches-50 | 8 | **15 %** | **5** | bulk noise |
| | **multiple-switches-100** | **16** | **10 %** | **10** | **stress gate** |
| **Button** | mount | 1 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 1 | 10 % | 3 | |
| | disabled | 1 | 10 % | 3 | |
| | with-color | 1 | 10 % | 3 | |
| | with-accessibility | 1 | 10 % | 3 | |
| | multiple-buttons-10 | 5 | 10 % | **5** | bulk noise |
| | multiple-buttons-50 | 26 | **15 %** | **5** | bulk noise |
| | **multiple-buttons-100** | **19** | **10 %** | **10** | **stress gate** |
| **ActivityIndicator** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0 | 10 % | 3 | |
| | multiple-indicators-10 | 1 | 10 % | **5** | bulk noise |
| | multiple-indicators-50 | 4 | **15 %** | **5** | bulk noise |
| | **multiple-indicators-100** | **7** | **10 %** | **10** | **stress gate** |
| **ScrollView** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0.5 | 10 % | 3 | |
| | with-children-20 | 3 | 10 % | 3 | |
| | with-children-100 | 15 | **15 %** | **5** | heavy |
| | horizontal | 3 | 10 % | 3 | |
| | sticky-headers | 3 | 10 % | 3 | |
| | nested-scroll-views | 1 | 10 % | **5** | |
| | **with-children-500** | **19** | **10 %** | **10** | **stress gate** |
| **Modal** | mount | 0 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 0.5 | 10 % | 3 | |
| | with-rich-content | 2 | 10 % | 3 | |

#### Interactive Components (3 suites)

| Component | Scenario | Baseline (ms) | Max Regression | Min Δ (ms) | Notes |
|---|---|---:|---|---:|---|
| **Pressable** | mount | 0.5 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 1 | 10 % | 3 | |
| | nested-pressables | 1 | 10 % | 3 | |
| | multiple-pressables-10 | 3 | 10 % | **5** | bulk noise |
| | multiple-pressables-50 | 15 | **15 %** | **5** | bulk noise |
| | **multiple-pressables-100** | **12** | **10 %** | **10** | **stress gate** |
| **TouchableOpacity** | mount | 1 | 10 % | 3 | |
| | rerender | 1.5 | 10 % | 3 | |
| | nested-touchables | 1.5 | 10 % | 3 | |
| | multiple-touchables-10 | 6 | 10 % | **5** | bulk noise |
| | multiple-touchables-50 | 29 | **15 %** | **5** | bulk noise |
| | **multiple-touchables-100** | **30** | **10 %** | **10** | **stress gate** |
| **TouchableHighlight** | mount | 1 | 10 % | 3 | |
| | rerender | 0.5 | 10 % | 3 | |
| | nested-touchables | 1 | 10 % | 3 | |
| | multiple-touchables-10 | 2 | 10 % | **5** | bulk noise |
| | multiple-touchables-50 | 12.5 | **15 %** | **5** | bulk noise |
| | **multiple-touchables-100** | **22.5** | **10 %** | **10** | **stress gate** |

#### List Components (2 suites)

| Component | Scenario | Baseline (ms) | Max Regression | Min Δ (ms) | Notes |
|---|---|---:|---|---:|---|
| **FlatList** | mount | 4 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 9 | 10 % | 3 | |
| | with-10-items | 4 | 10 % | 3 | |
| | with-100-items | 5 | 10 % | **5** | |
| | with-500-items | 5 | **15 %** | **10** | large list |
| | horizontal | 4.5 | 10 % | **5** | |
| | with-separator | 6 | 10 % | **5** | |
| | with-header-footer | 2 | 10 % | **5** | |
| | with-empty-list | 1 | 10 % | 3 | |
| | with-get-item-layout | 2 | 10 % | **5** | |
| | inverted | 2 | 10 % | **5** | |
| | **with-1000-items** | **4** | **15 %** | **10** | **stress gate (virtualized)** |
| | with-num-columns | 3 | 10 % | **5** | |
| **SectionList** | mount | 5 | 10 % | 3 | |
| | unmount | 0 | 10 % | 3 | |
| | rerender | 11 | 10 % | 3 | |
| | 3-sections × 5-items | 5 | 10 % | **5** | |
| | 5-sections × 10-items | 6 | 10 % | **5** | |
| | 10-sections × 20-items | 5.5 | **15 %** | **10** | 200 items |
| | 20-sections × 10-items | 5.5 | **15 %** | **10** | 200 items |
| | with-section-separator | — | 10 % | **5** | |
| | with-item-separator | — | 10 % | **5** | |
| | with-header-footer | — | 10 % | **5** | |
| | with-section-footer | — | 10 % | **5** | |
| | sticky-section-headers | — | 10 % | **5** | |
| | **with-50-sections-20-items** | **2** | **15 %** | **10** | **stress gate (virtualized)** |
| | with-empty-list | 0 | 10 % | 3 | |

### Why these numbers?

| Design choice | Rationale |
|---|---|
| **10 % default gate** | Sub-millisecond components need tight gates — a 0 ms→1 ms jump is ∞ % but only 1 ms. The 3 ms absolute floor prevents that from failing. The 10 % gate catches meaningful shifts in the 5-30 ms range. |
| **15 % for bulk/heavy scenarios** | Rendering 50-100+ elements has inherently higher variance from GC pauses, thread scheduling. A tighter gate would flake. |
| **3 ms vs 5 ms vs 10 ms absolute floors** | Simple atoms (View, Text) fluctuate < 1 ms — a 3 ms floor is plenty. Bulk scenarios with σ > 1 ms need a 5 ms floor. 500-item lists need 10 ms to avoid noise trips. |
| **Max CV 0.40-0.50** | If stdDev/mean > 0.50 the measurement is too noisy to compare. The Mann-Whitney U test still runs but the CV gate warns first. Core preset is stricter (0.40) because simple components should be stable. |
| **gate vs track** | All first-party components use `gate` (fail CI). The `community` preset exists for third-party components where the team can monitor trends without blocking PRs. |
| **Mann-Whitney over t-test** | Render times are right-skewed (occasional GC spikes), not normally distributed. Mann-Whitney is distribution-free and rank-based — proven in Google Catapult for exactly this use case. |
| **Min 10 runs** | At n=10, Mann-Whitney achieves α=0.05 with power ≈ 0.80 for medium effect sizes. Fewer runs increase false-negative risk. List tests allow 5 runs because each run is expensive. |
