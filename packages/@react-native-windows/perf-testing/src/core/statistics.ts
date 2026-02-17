/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

export function mean(values: number[]): number {
  if (values.length === 0) {
    return 0;
  }
  return values.reduce((sum, v) => sum + v, 0) / values.length;
}

export function median(values: number[]): number {
  if (values.length === 0) {
    return 0;
  }
  const sorted = [...values].sort((a, b) => a - b);
  const mid = Math.floor(sorted.length / 2);
  return sorted.length % 2 !== 0
    ? sorted[mid]
    : (sorted[mid - 1] + sorted[mid]) / 2;
}

export function standardDeviation(values: number[]): number {
  if (values.length < 2) {
    return 0;
  }
  const avg = mean(values);
  const squaredDiffs = values.map(v => (v - avg) ** 2);
  return Math.sqrt(
    squaredDiffs.reduce((sum, v) => sum + v, 0) / (values.length - 1),
  );
}

export function coefficientOfVariation(values: number[]): number {
  if (values.length < 2) {
    return 0;
  }
  const avg = mean(values);
  if (avg === 0) {
    return 0;
  }
  return standardDeviation(values) / avg;
}

/**
 * Normal CDF approximation (Abramowitz & Stegun 26.2.17).
 * Accurate to ~1.5e-7 for all z.
 */
function normalCDF(z: number): number {
  if (z < -8) {
    return 0;
  }
  if (z > 8) {
    return 1;
  }

  const a1 = 0.254829592;
  const a2 = -0.284496736;
  const a3 = 1.421413741;
  const a4 = -1.453152027;
  const a5 = 1.061405429;
  const p = 0.3275911;

  const sign = z < 0 ? -1 : 1;
  const x = Math.abs(z) / Math.sqrt(2);
  const t = 1.0 / (1.0 + p * x);
  const y =
    1.0 - ((((a5 * t + a4) * t + a3) * t + a2) * t + a1) * t * Math.exp(-x * x);

  return 0.5 * (1.0 + sign * y);
}

export interface MannWhitneyResult {
  U: number;
  z: number;
  p: number;
  significant: boolean;
}

/**
 * Mann-Whitney U test (one-tailed: is sample2 significantly greater?).
 *
 * Non-parametric test appropriate for perf measurements that may not
 * follow a normal distribution. Uses the normal approximation with
 * tie correction, valid for n >= 8.
 *
 * @param sample1 - Baseline durations
 * @param sample2 - Current (head) durations
 * @param alpha - Significance level (default 0.05)
 * @returns Test result with U statistic, z-score, p-value, and significance flag
 */
export function mannWhitneyU(
  sample1: number[],
  sample2: number[],
  alpha: number = 0.05,
): MannWhitneyResult {
  const n1 = sample1.length;
  const n2 = sample2.length;

  if (n1 < 2 || n2 < 2) {
    return {U: 0, z: 0, p: 1, significant: false};
  }

  // Tag each value with its group and combine
  const combined: Array<{value: number; group: number}> = [
    ...sample1.map(v => ({value: v, group: 1})),
    ...sample2.map(v => ({value: v, group: 2})),
  ];

  // Sort by value
  combined.sort((a, b) => a.value - b.value);

  // Assign ranks with tie handling (average rank for ties)
  const ranks = new Array<number>(combined.length);
  let i = 0;
  while (i < combined.length) {
    let j = i;
    while (j < combined.length && combined[j].value === combined[i].value) {
      j++;
    }
    const avgRank = (i + 1 + j) / 2;
    for (let k = i; k < j; k++) {
      ranks[k] = avgRank;
    }
    i = j;
  }

  // Sum of ranks for sample 1
  let R1 = 0;
  for (let idx = 0; idx < combined.length; idx++) {
    if (combined[idx].group === 1) {
      R1 += ranks[idx];
    }
  }

  // U statistics
  const U1 = n1 * n2 + (n1 * (n1 + 1)) / 2 - R1;
  const U2 = n1 * n2 - U1;
  const U = Math.min(U1, U2);

  // Normal approximation with tie correction
  const n = n1 + n2;
  const meanU = (n1 * n2) / 2;

  // Calculate tie correction factor
  let tieCorrection = 0;
  let idx = 0;
  while (idx < combined.length) {
    let tieCount = 1;
    while (
      idx + tieCount < combined.length &&
      combined[idx + tieCount].value === combined[idx].value
    ) {
      tieCount++;
    }
    if (tieCount > 1) {
      tieCorrection += tieCount ** 3 - tieCount;
    }
    idx += tieCount;
  }

  const sigmaU = Math.sqrt(
    (n1 * n2 * (n + 1)) / 12 - (n1 * n2 * tieCorrection) / (12 * n * (n - 1)),
  );

  if (sigmaU === 0) {
    return {U, z: 0, p: 1, significant: false};
  }

  // One-tailed: is sample2 significantly greater than sample1?
  // Lower U1 means sample2 tends to have higher ranks
  const z = (U - meanU) / sigmaU;
  const p = normalCDF(z);

  return {U, z, p, significant: p < alpha};
}
