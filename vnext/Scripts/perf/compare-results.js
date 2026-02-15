/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Compare perf test results from the current (head) run against
 * committed baselines from the base branch. Produces a markdown
 * comparison report and exits with non-zero if regressions detected.
 *
 * Usage:
 *   node vnext/Scripts/perf/compare-results.js [options]
 *
 * Options:
 *   --results <path>    Path to CI results JSON (default: .perf-results/results.json)
 *   --baselines <dir>   Path to base branch perf snapshots directory
 *   --output <path>     Path to write markdown report (default: .perf-results/report.md)
 *   --fail-on-regression  Exit 1 if regressions found (default: true in CI)
 *
 * @format
 */

'use strict';

const fs = require('fs');
const path = require('path');

// ---------------------------------------------------------------------------
// Argument parsing
// ---------------------------------------------------------------------------

function parseArgs() {
  const args = process.argv.slice(2);
  const opts = {
    results: '.perf-results/results.json',
    baselines: null, // auto-detect from test file paths
    output: '.perf-results/report.md',
    failOnRegression: !!process.env.CI,
  };

  for (let i = 0; i < args.length; i++) {
    switch (args[i]) {
      case '--results':
        opts.results = args[++i];
        break;
      case '--baselines':
        opts.baselines = args[++i];
        break;
      case '--output':
        opts.output = args[++i];
        break;
      case '--fail-on-regression':
        opts.failOnRegression = true;
        break;
      case '--no-fail-on-regression':
        opts.failOnRegression = false;
        break;
    }
  }
  return opts;
}

// ---------------------------------------------------------------------------
// Snapshot loading helpers
// ---------------------------------------------------------------------------

/**
 * Resolve the __perf_snapshots__ path for a given test file.
 */
function getSnapshotPath(testFilePath) {
  const dir = path.dirname(testFilePath);
  const basename = path.basename(testFilePath);
  return path.join(dir, '__perf_snapshots__', `${basename}.perf-baseline.json`);
}

/**
 * Load a .perf.snap JSON file.
 */
function loadSnapshot(filePath) {
  if (fs.existsSync(filePath)) {
    return JSON.parse(fs.readFileSync(filePath, 'utf-8'));
  }
  return {};
}

// ---------------------------------------------------------------------------
// Comparison engine (mirrors BaselineComparator logic)
// ---------------------------------------------------------------------------

const DEFAULT_THRESHOLD = {
  maxDurationIncrease: 10,
  maxDuration: Infinity,
  minAbsoluteDelta: 3,
  maxRenderCount: 5,
  minRuns: 10,
};

/**
 * Restore Infinity values that were serialized as null in JSON.
 */
function resolveThreshold(threshold) {
  return {
    ...DEFAULT_THRESHOLD,
    ...threshold,
    // JSON.stringify(Infinity) === 'null', so restore it
    maxDuration:
      threshold.maxDuration === null || threshold.maxDuration === undefined
        ? Infinity
        : threshold.maxDuration,
  };
}

function compareEntry(head, base, threshold) {
  // Use median for comparison — robust to outlier spikes
  const percentChange =
    base.medianDuration > 0
      ? ((head.medianDuration - base.medianDuration) / base.medianDuration) *
        100
      : 0;

  const errors = [];

  const absoluteDelta = head.medianDuration - base.medianDuration;
  const minAbsoluteDelta =
    threshold.minAbsoluteDelta ?? DEFAULT_THRESHOLD.minAbsoluteDelta;
  if (
    percentChange > threshold.maxDurationIncrease &&
    absoluteDelta > minAbsoluteDelta
  ) {
    errors.push(
      `Duration increased by ${percentChange.toFixed(1)}% / +${absoluteDelta.toFixed(2)}ms (threshold: ${threshold.maxDurationIncrease}% & ${minAbsoluteDelta}ms)`,
    );
  }
  if (head.medianDuration > threshold.maxDuration) {
    errors.push(
      `Duration ${head.medianDuration.toFixed(2)}ms exceeds max ${threshold.maxDuration}ms`,
    );
  }
  if (head.renderCount > threshold.maxRenderCount) {
    errors.push(
      `Render count ${head.renderCount} exceeds max ${threshold.maxRenderCount}`,
    );
  }

  return {
    name: head.name,
    head,
    base,
    percentChange,
    passed: errors.length === 0,
    errors,
  };
}

function compareSuite(suiteName, headSnaps, baseSnaps) {
  const results = [];

  for (const [key, headEntry] of Object.entries(headSnaps)) {
    const baseEntry = baseSnaps[key];
    const threshold = resolveThreshold({
      ...DEFAULT_THRESHOLD,
      ...(headEntry.threshold || {}),
    });

    if (!baseEntry) {
      results.push({
        name: headEntry.metrics.name,
        head: headEntry.metrics,
        base: null,
        percentChange: null,
        passed: true,
        errors: [],
        isNew: true,
      });
      continue;
    }

    results.push(compareEntry(headEntry.metrics, baseEntry.metrics, threshold));
  }

  // Detect removed scenarios
  for (const key of Object.keys(baseSnaps)) {
    if (!headSnaps[key]) {
      results.push({
        name: `[REMOVED] ${baseSnaps[key].metrics.name}`,
        head: null,
        base: baseSnaps[key].metrics,
        percentChange: null,
        passed: true,
        errors: [],
        isRemoved: true,
      });
    }
  }

  return {suiteName, results, hasRegressions: results.some(r => !r.passed)};
}

// ---------------------------------------------------------------------------
// Markdown generation
// ---------------------------------------------------------------------------

function generateMarkdown(suiteComparisons, ciResults) {
  let md = '## ⚡ Performance Test Results\n\n';

  md += `**Branch:** \`${ciResults.branch}\`  \n`;
  md += `**Commit:** \`${ciResults.commitSha.slice(0, 8)}\`  \n`;
  md += `**Time:** ${ciResults.timestamp}  \n`;
  md += `**Tests:** ${ciResults.summary.passed}/${ciResults.summary.totalTests} passed  \n\n`;

  const allRegressions = suiteComparisons.filter(s => s.hasRegressions);
  if (allRegressions.length > 0) {
    md += '### ❌ Regressions Detected\n\n';
    for (const suite of allRegressions) {
      md += `#### ${suite.suiteName}\n\n`;
      md += '| Scenario | Baseline | Current | Change | Status |\n';
      md += '|----------|----------|---------|--------|--------|\n';
      for (const r of suite.results.filter(x => !x.passed)) {
        const baseline = r.base ? `${r.base.meanDuration.toFixed(2)}ms` : 'N/A';
        const current = r.head ? `${r.head.meanDuration.toFixed(2)}ms` : 'N/A';
        const change =
          r.percentChange != null ? `+${r.percentChange.toFixed(1)}%` : 'N/A';
        md += `| ${r.name} | ${baseline} | ${current} | ${change} | ❌ |\n`;
      }
      md += '\n';
      for (const r of suite.results.filter(x => !x.passed)) {
        if (r.errors.length > 0) {
          md += `> **${r.name}:** ${r.errors.join('; ')}  \n`;
        }
      }
      md += '\n';
    }
  }

  // Passed suites
  const passedSuites = suiteComparisons.filter(s => !s.hasRegressions);
  if (passedSuites.length > 0) {
    md += '### ✅ Passed\n\n';
    md += '<details>\n<summary>';
    const totalPassed = passedSuites.reduce(
      (acc, s) => acc + s.results.length,
      0,
    );
    md += `${totalPassed} scenario(s) across ${passedSuites.length} suite(s) — no regressions`;
    md += '</summary>\n\n';

    for (const suite of passedSuites) {
      md += `#### ${suite.suiteName}\n\n`;
      md += '| Scenario | Mean | Median | StdDev | Renders | vs Baseline |\n';
      md += '|----------|------|--------|--------|---------|-------------|\n';
      for (const r of suite.results) {
        if (!r.head) continue;
        const change =
          r.percentChange != null
            ? `${r.percentChange >= 0 ? '+' : ''}${r.percentChange.toFixed(1)}%`
            : 'new';
        md +=
          `| ${r.name}` +
          ` | ${r.head.meanDuration.toFixed(2)}ms` +
          ` | ${r.head.medianDuration.toFixed(2)}ms` +
          ` | ±${r.head.stdDev.toFixed(2)}ms` +
          ` | ${r.head.renderCount}` +
          ` | ${change} |\n`;
      }
      md += '\n';
    }
    md += '</details>\n';
  }

  // New scenarios
  const allNew = suiteComparisons.flatMap(s => s.results.filter(r => r.isNew));
  if (allNew.length > 0) {
    md += '\n### 🆕 New Scenarios\n\n';
    md += `${allNew.length} new scenario(s) added (no baseline comparison).  \n`;
    md +=
      'These will become baselines after merge. Run `yarn perf:update` to capture locally.\n';
  }

  return md;
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

function main() {
  const opts = parseArgs();

  // 1. Load CI results JSON
  if (!fs.existsSync(opts.results)) {
    console.error(`❌ Results file not found: ${opts.results}`);
    console.error('Run perf tests with CI=true first: CI=true yarn perf:ci');
    process.exit(1);
  }

  const ciResults = JSON.parse(fs.readFileSync(opts.results, 'utf-8'));
  console.log(
    `📊 Loaded ${ciResults.suites.length} suite(s) from ${opts.results}`,
  );

  // 2. Compare each suite against its committed baseline
  const suiteComparisons = [];

  for (const suite of ciResults.suites) {
    // Head snapshots come from the CI run
    const headSnaps = suite.snapshots || {};

    // Base snapshots come from the committed .perf.snap files
    // In CI, the base branch is checked out into a separate dir,
    // or we read the committed snapshots from the test file path
    let baseSnapPath;
    if (opts.baselines) {
      // Explicit baseline directory — look for matching snap file
      const relPath = path.relative(process.cwd(), suite.testFilePath);
      baseSnapPath = path.join(
        opts.baselines,
        path.dirname(relPath),
        '__perf_snapshots__',
        `${path.basename(suite.testFilePath)}.perf.snap`,
      );
    } else {
      // Default: baselines live adjacent to test files (committed in repo)
      baseSnapPath = getSnapshotPath(suite.testFilePath);
    }

    const baseSnaps = loadSnapshot(baseSnapPath);
    console.log(
      `  📋 ${suite.suiteName}: ${Object.keys(headSnaps).length} head / ${Object.keys(baseSnaps).length} base snapshots`,
    );

    suiteComparisons.push(compareSuite(suite.suiteName, headSnaps, baseSnaps));
  }

  // 3. Generate markdown report
  const markdown = generateMarkdown(suiteComparisons, ciResults);

  const outputDir = path.dirname(opts.output);
  if (!fs.existsSync(outputDir)) {
    fs.mkdirSync(outputDir, {recursive: true});
  }
  fs.writeFileSync(opts.output, markdown, 'utf-8');
  console.log(`\n📝 Report written to ${opts.output}`);

  // 4. Also write a machine-readable summary
  const summaryPath = opts.output.replace(/\.md$/, '.json');
  const summaryData = {
    hasRegressions: suiteComparisons.some(s => s.hasRegressions),
    totalSuites: suiteComparisons.length,
    totalScenarios: suiteComparisons.reduce(
      (acc, s) => acc + s.results.length,
      0,
    ),
    regressions: suiteComparisons.flatMap(s =>
      s.results
        .filter(r => !r.passed)
        .map(r => ({
          suite: s.suiteName,
          scenario: r.name,
          percentChange: r.percentChange,
          errors: r.errors,
        })),
    ),
  };
  fs.writeFileSync(summaryPath, JSON.stringify(summaryData, null, 2) + '\n');

  // 5. Print summary
  const hasRegressions = suiteComparisons.some(s => s.hasRegressions);
  if (hasRegressions) {
    const count = summaryData.regressions.length;
    console.error(`\n❌ ${count} regression(s) detected!`);
    for (const reg of summaryData.regressions) {
      console.error(
        `  - ${reg.suite} / ${reg.scenario}: ${reg.errors.join('; ')}`,
      );
    }
    if (opts.failOnRegression) {
      process.exit(1);
    }
  } else {
    console.log(
      `\n✅ All ${summaryData.totalScenarios} scenarios passed — no regressions.`,
    );
  }
}

main();
