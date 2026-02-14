/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {PerfMetrics} from '../interfaces/PerfMetrics';

/**
 * Reporter that outputs perf results to the console.
 *
 * Used as the default reporter during local development.
 */
export class ConsoleReporter {
  /**
   * Print a summary of multiple perf results.
   */
  static report(results: PerfMetrics[]): void {
    console.log('\n⚡ Performance Test Results\n');
    console.log(
      '─'.repeat(72),
    );
    console.log(
      padRight('Scenario', 30) +
        padRight('Mean (ms)', 12) +
        padRight('Median (ms)', 13) +
        padRight('StdDev', 10) +
        padRight('Renders', 8),
    );
    console.log(
      '─'.repeat(72),
    );

    for (const result of results) {
      console.log(
        padRight(result.name, 30) +
          padRight(result.meanDuration.toFixed(2), 12) +
          padRight(result.medianDuration.toFixed(2), 13) +
          padRight(result.stdDev.toFixed(2), 10) +
          padRight(String(result.renderCount), 8),
      );
    }

    console.log(
      '─'.repeat(72),
    );
    console.log(`\nTotal scenarios: ${results.length}`);
    console.log(`Timestamp: ${new Date().toISOString()}\n`);
  }

  /**
   * Print a single result line.
   */
  static reportSingle(result: PerfMetrics): void {
    console.log(
      `  ${result.name}: ` +
        `${result.meanDuration.toFixed(2)}ms (±${result.stdDev.toFixed(2)}ms) ` +
        `[${result.renderCount} renders, ${result.runs} runs]`,
    );
  }
}

function padRight(str: string, len: number): string {
  return str.length >= len ? str : str + ' '.repeat(len - str.length);
}
