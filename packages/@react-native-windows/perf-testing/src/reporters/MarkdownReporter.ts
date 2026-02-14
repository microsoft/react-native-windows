/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {PerfMetrics} from '../interfaces/PerfMetrics';

/**
 * Comparison info used when generating markdown reports.
 */
export interface ComparisonResult {
  metrics: PerfMetrics;
  baselineMetrics?: PerfMetrics;
  percentChange?: number;
  passed: boolean;
  error?: string;
}

/**
 * Reporter that generates Markdown-formatted perf results.
 *
 * Used for CI/CD PR comments and artifact reports.
 */
export class MarkdownReporter {
  /**
   * Generate a full markdown report from comparison results.
   */
  static generate(comparisons: ComparisonResult[]): string {
    const passed = comparisons.filter(c => c.passed);
    const failed = comparisons.filter(c => !c.passed);

    let md = '';

    if (failed.length > 0) {
      md += '### ❌ Regressions\n\n';
      md += '| Scenario | Baseline | Current | Change | Status |\n';
      md += '|----------|----------|---------|--------|--------|\n';

      for (const f of failed) {
        const baseline = f.baselineMetrics
          ? `${f.baselineMetrics.meanDuration.toFixed(2)}ms`
          : 'N/A';
        const current = `${f.metrics.meanDuration.toFixed(2)}ms`;
        const change =
          f.percentChange !== undefined
            ? `+${f.percentChange.toFixed(1)}%`
            : 'N/A';
        md += `| ${f.metrics.name} | ${baseline} | ${current} | ${change} | ❌ |\n`;
      }
      md += '\n';
    }

    if (passed.length > 0) {
      md += '### ✅ Passed\n\n';
      md += `${passed.length} test(s) passed\n\n`;
      md += '<details>\n<summary>Show details</summary>\n\n';
      md += '| Scenario | Mean | Median | StdDev | Renders |\n';
      md += '|----------|------|--------|--------|---------|\n';

      for (const p of passed) {
        md +=
          `| ${p.metrics.name}` +
          ` | ${p.metrics.meanDuration.toFixed(2)}ms` +
          ` | ${p.metrics.medianDuration.toFixed(2)}ms` +
          ` | ±${p.metrics.stdDev.toFixed(2)}ms` +
          ` | ${p.metrics.renderCount} |\n`;
      }
      md += '\n</details>\n';
    }

    return md;
  }

  /**
   * Generate a short summary line for use in PR comments.
   */
  static summary(comparisons: ComparisonResult[]): string {
    const passed = comparisons.filter(c => c.passed).length;
    const failed = comparisons.filter(c => !c.passed).length;
    const total = comparisons.length;

    if (failed > 0) {
      return `❌ ${failed}/${total} perf tests failed — performance regressions detected.`;
    }
    return `✅ All ${passed} perf tests passed.`;
  }
}
